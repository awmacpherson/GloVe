//  Tool to extract unigram counts
//
//  GloVe: Global Vectors for Word Representation
//  Copyright (c) 2014 The Board of Trustees of
//  The Leland Stanford Junior University. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//
//  For more information, bug reports, fixes, contact:
//    Jeffrey Pennington (jpennin@stanford.edu)
//    Christopher Manning (manning@cs.stanford.edu)
//    https://github.com/stanfordnlp/GloVe/
//    GlobalVectors@googlegroups.com
//    http://nlp.stanford.edu/projects/glove/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

typedef struct vocabulary {
    char *word;
    int code;
    long long count;
} VOCAB;

int verbose = 2; // 0, 1, or 2
long long min_count = 1; // min occurrences for inclusion in vocab
long long max_vocab = 0; // max_vocab = 0 for no limit
int vocab_count = 0;


/* Vocab frequency comparison; break ties alphabetically */
int CompareVocabTie(const void *a, const void *b) {
    long long c;
    if ( (c = ((VOCAB *) b)->count - ((VOCAB *) a)->count) != 0) return ( c > 0 ? 1 : -1 );
    else return (scmp(((VOCAB *) a)->word,((VOCAB *) b)->word));
    
}

/* Vocab frequency comparison; no tie-breaker */
int CompareVocab(const void *a, const void *b) {
    long long c;
    if ( (c = ((VOCAB *) b)->count - ((VOCAB *) a)->count) != 0) return ( c > 0 ? 1 : -1 );
    else return 0;
}

/* Search hash table for given string, insert if not found */
int hashinsert(HASHREC **ht, char *w) {
    HASHREC     *htmp, *hprv;
    unsigned int hval = HASHFN(w, TSIZE, SEED);
    
    for (hprv = NULL, htmp = ht[hval]; htmp != NULL && scmp(htmp->word, w) != 0; hprv = htmp, htmp = htmp->next) ;
    
    if (htmp == NULL) {
        htmp = (HASHREC *) malloc( sizeof(HASHREC) );
        htmp->word = (char *) malloc( strlen(w) + 1 );
        strcpy(htmp->word, w);
        htmp->num = 1;
        htmp->code = vocab_count++;
        htmp->next = NULL;
        if ( hprv==NULL )
            ht[hval] = htmp;
        else
            hprv->next = htmp;
        return htmp->code;
    }
    else {
        /* new records are not moved to front */
        htmp->num++;
        if (hprv != NULL) {
            /* move to front on access */
            hprv->next = htmp->next;
            htmp->next = ht[hval];
            ht[hval] = htmp;
        }
        return htmp->code;
    }
}

#define BUFSIZE 134217728
// int BUFSIZE = 1073741824;

int serialize_vocab(VOCAB * vocab, long long max_vocab, long long min_count) {
    
    int i;
    for (i = 0; i < max_vocab; i++) {
        if (vocab[i].count < min_count) {
            // If a minimum frequency cutoff exists, truncate vocabulary
            if (verbose > 0) 
                fprintf(stderr, "Truncating vocabulary at min count %lld.\n",min_count);
            break;
        }
        printf("%s %d\n", vocab[i].word, vocab[i].code);
        // warning: prints a newline at the end.
    }
    
    if (i == max_vocab && verbose > 0) 
        fprintf(stderr, "Truncating vocabulary at size %lld.\n", max_vocab);
    
    return i; // number of tokens
}

/* 
    Definition of the behaviour:
    Assigns non-negative integer codes to words in the order they are
    encountered (type signed int). Codes are written to a buffer which is 
    flushed to disk (as binary) every BUFSIZE tokens. Newlines trigger a -1
    to be written to the buffer.

    Codes are also recorded in the HASHREC struct and seriealized in the form
    <word> <count> <code>\n
    in the vocab file.
*/

int get_counts(void) {
    long long i = 0, j = 0, vocab_size = 12500;
    // char format[20];
    char str[MAX_STRING_LENGTH + 1];
    HASHREC **vocab_hash = inithashtable();
    HASHREC *htmp;
    VOCAB *vocab;
    FILE * fid = stdin;
    FILE * encoded_file;
    if (!(encoded_file = fopen("encoded", "wb"))) return 1;
    int * encoded = (int *) malloc(BUFSIZE * sizeof(int));
    int * encodedp = encoded;    
    size_t num_words = 0;
    
    fprintf(stderr, "BUILDING VOCABULARY\n");
    while ( ! feof(fid)) {
        // Insert all tokens into hashtable
	    if ( get_word3(str, fid) )  // just a newline marker or feof
            *encodedp++ = -1;
		else 
			*encodedp++ = hashinsert(vocab_hash, str);
		
        if (((encodedp-encoded) % BUFSIZE) == 0) {
            num_words += fwrite(encoded, sizeof(int), BUFSIZE, encoded_file);
            if (verbose > 1) fprintf(stderr, "Written %zd encoded tokens to disk.\n", num_words);
            encodedp = encoded;
        }
    }
    //if (verbose > 1) fprintf(stderr, "\033[0GProcessed %lld tokens.\n", );
    if (encodedp != encoded) {
        num_words = fwrite(encoded, sizeof(int), *(encodedp-1) - *encoded + 1, encoded_file);
        fprintf(stderr, "Final chunk: wrote %zd tokens to disk.\n", num_words);
    }
        
    
    fclose(encoded_file);
    
    vocab = malloc(sizeof(VOCAB) * vocab_size);
    for (i = 0; i < TSIZE; i++) { // Migrate vocab to array
        htmp = vocab_hash[i];
        while (htmp != NULL) {
            vocab[j].word = htmp->word;
            vocab[j].code = htmp->code;
            vocab[j].count = htmp->num;
            j++;
            if (j>=vocab_size) {
                vocab_size += 2500;
                vocab = (VOCAB *)realloc(vocab, sizeof(VOCAB) * vocab_size);
            }
            htmp = htmp->next;
        }
    }
    if (verbose > 1) fprintf(stderr, "Counted %lld unique words.\n", j);

    // SORT BY FREQUENCY
    if (max_vocab > 0 && max_vocab < j)
    	
        /* If the vocabulary exceeds limit, first sort full vocab by frequency 
         * without alphabetical tie-breaks.
         * This results in pseudo-random ordering for words with same frequency, 
         * so that when truncated, the words span whole alphabet */

        qsort(vocab, j, sizeof(VOCAB), CompareVocab);

    else max_vocab = j;
    qsort(vocab, max_vocab, sizeof(VOCAB), CompareVocabTie); 
    //After (possibly) truncating, sort (possibly again), breaking ties alphabetically
    
    // TRUNCATE AND SERIALIZE
    i = serialize_vocab(vocab, max_vocab, min_count);
    fprintf(stderr, "Using vocabulary of size %lld.\n\n", i);
    
    // CLEAN UP
    free_table(vocab_hash);
    free(vocab);
    
    return 0;
}


int main(int argc, char **argv) {
    if (argc == 2 &&
        (!scmp(argv[1], "-h") || !scmp(argv[1], "-help") || !scmp(argv[1], "--help"))) {
        printf("Simple tool to extract unigram counts\n");
        printf("Author: Jeffrey Pennington (jpennin@stanford.edu)\n\n");
        printf("Usage options:\n");
        printf("\t-verbose <int>\n");
        printf("\t\tSet verbosity: 0, 1, or 2 (default)\n");
        printf("\t-max-vocab <int>\n");
        printf("\t\tUpper bound on vocabulary size, i.e. keep the <int> most frequent words. The minimum frequency words are randomly sampled so as to obtain an even distribution over the alphabet.\n");
        printf("\t-min-count <int>\n");
        printf("\t\tLower limit such that words which occur fewer than <int> times are discarded.\n");
        printf("\nExample usage:\n");
        printf("./vocab_count -verbose 2 -max-vocab 100000 -min-count 10 < corpus.txt > vocab.txt\n");
        return 0;
    }

    int i;
    if ((i = find_arg((char *)"-verbose", argc, argv)) > 0) verbose = atoi(argv[i + 1]);
    if ((i = find_arg((char *)"-max-vocab", argc, argv)) > 0) max_vocab = atoll(argv[i + 1]);
    if ((i = find_arg((char *)"-min-count", argc, argv)) > 0) min_count = atoll(argv[i + 1]);
    return get_counts();
}

