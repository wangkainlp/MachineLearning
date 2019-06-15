#coding:utf8

from ctypes import *

class StructPointer(Structure):
    _fields_ = [("first", c_float), ("second", c_float)]


#load the shared object file
adder = CDLL('./util.so')
# adder = cdll.LoadLibrary('./adder.so')



'''
#Find sum of integers
res_int = adder.add_int(4,5)
print "Sum of 4 and 5 = " + str(res_int)

#Find sum of floats
a = c_float(5.5)
b = c_float(4.1)

add_float = adder.add_float
add_float.restype = c_float
print "Sum of 5.5 and 4.1 = ", str(add_float(a, b))
'''


float_array_10 = c_float * 10
array = float_array_10()
for i in range(5):
    array[i] = c_float(i)

adder.add_arr.restype = c_float
adder.variance.restype = c_float

print adder.add_arr(array, 5)
print adder.variance(array, 5)

# adder.test_tuple.argtypes = (c_int, c_int)
adder.test_tuple.restype = POINTER(StructPointer)

adder.get_tuple.argtype = POINTER(StructPointer)
adder.get_tuple.restype = c_float

adder.free_tuple.argtype = POINTER(StructPointer)
adder.free_tuple.restype = None

adder.findLeastVar.restype = POINTER(StructPointer)

size = 10
spList = [ i + 0.5 for i in range(9) ]
labels = [ i % 2 for i in range(10) ]
feaList = [ i for i in range(10) ]


p = adder.findLeastVar((c_float * size)(* spList), 
                       (c_float * size)(* labels),
                       (c_float * size)(* feaList),
                       c_int(size) )
print p.contents.first, p.contents.second

print adder.get_tuple(p)
adder.free_tuple(p)
print p.contents.first
print adder.get_tuple(p)

'''

print "get", adder.get_tuple(p)
adder.free_tuple(p)

print p.contents.first
print "get", adder.get_tuple(p)
'''
