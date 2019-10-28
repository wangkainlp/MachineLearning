#build the modules
from distutils.core import setup, Extension

setup(name='var', version='1.0',  \
      ext_modules=[Extension('var', ['var.cpp'])])

