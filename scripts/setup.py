from setuptools import setup, Extension
from Cython.Build import cythonize

reencode = Extension("reencode", sources=["reencode.c"])
twogram = Extension("twogram", sources=["twogram.c"])

setup(ext_modules = [reencode, twogram])
