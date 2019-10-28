from distutils.core import setup, Extension
import os

os.environ["CC"] = "gcc"
os.environ["CXX"] = "g++"

module = Extension('spam', sources = ['spammify.c'])

module_tree = Extension('tree',
                        language = "c++",
                        include_dirs = ['.'],
                        # extra_compile_args = ['--std=c++0x'],
                        # extra_compile_args=['-Wno-unused-function', '-std=c++11'],
                        # extra_compile_args = ['-std=c++11'],
                        # sources = ['treemify.cpp', 'myutil.cpp' ])
                        sources = ['treemify.cpp'])

setup ( name = 'test',
        version = '1.0',
        description = 'test pkg: tree',
        ext_modules = [module_tree])

'''
os.environ["CC"] = "gcc-4.8"
os.environ["CXX"] = "gcc-4.8"
'''


"""
from distutils.core import setup, Extension

module1 = Extension('demo',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = ['/usr/local/include'],
                    libraries = ['tcl83'],
                    library_dirs = ['/usr/local/lib'],
                    sources = ['demo.c'])

setup (name = 'PackageName',
       version = '1.0',
       description = 'This is a demo package',
       author = 'Martin v. Loewis',
       author_email = 'martin@v.loewis.de',
       url = 'https://docs.python.org/extending/building',
       long_description = ''' This is really just a demo package. ''',
       ext_modules = [module1])
"""
