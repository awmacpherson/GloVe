from Cython.Build.BuildExecutable import build
from os.path import join

SCRIPTSDIR = "scripts"

remap = build(join(SCRIPTSDIR, "remap.pyx"))
twogram = build(join(SCRIPTSDIR, "twogram.pyx"))
