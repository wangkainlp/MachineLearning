
rm -f build/lib.macosx-10.14-intel-2.7/tree.so
/usr/bin/python setup.py build_ext

[ -e ./tree.so ] && rm -f ./tree.so
# cp build/lib.macosx-10.6-x86_64-2.7/tree.so .
cp build/lib.macosx-10.14-intel-2.7/tree.so .
