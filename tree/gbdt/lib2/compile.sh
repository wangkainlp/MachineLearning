rm -rf build
# /home/work/.jumbo/bin/python2.7 setup.py build_ext
# [ $? -eq 0 ] && { cp build/lib.linux-x86_64-2.7/tree.so ../tree.so; }

/usr/bin/python setup.py build_ext
# [ $? -eq 0 ] && { cp build/lib.linux-x86_64-2.7/tree.so ../tree.so; }
