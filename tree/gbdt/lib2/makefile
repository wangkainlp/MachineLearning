
CXX = g++
CXXFLAGS = -std=c++11 # -arch x86_64

PY_LIB = /System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7
INC = -I . -I $(PY_LIB)

MAC_EXTRA_LINK_ARGS = -Wl,-undefined,dynamic_lookup

# tree.so : $(obj)
tree.so : treemify.o
	$(CXX) $(CXXFLAGS) -shared $(MAC_EXTRA_LINK_ARGS) $^ -o tree.so

treemify.o : treemify.cpp 
	$(CXX) $(CXXFLAGS) $(INC) -fPIC -c $^ -o $@

# common.o : common.h
# 	$(CXX) $(CXXFLAGS) -I $(PY_LIB) -c $^ -o $@
# 
# data.o : data.h
# 	$(CXX) $(CXXFLAGS) -I $(PY_LIB) -c $^ -o $@
# 
# loss.o : loss.h
# 	$(CXX) $(CXXFLAGS) -I $(PY_LIB) -c $^ -o $@
# 
# myutil.o : myutil.h
# 	$(CXX) $(CXXFLAGS) -I $(PY_LIB) -c $^ -o $@
# 
# sample.o : sample.h
# 	$(CXX) $(CXXFLAGS) -I $(PY_LIB) -c $^ -o $@
# 
# tree_node.o : tree_node.h
# 	$(CXX) $(CXXFLAGS) -I $(PY_LIB) -c $^ -o $@
# 
# gbdt_tree.o : gbdt_tree.h
# 	$(CXX) $(CXXFLAGS) -I $(PY_LIB) -c $^ -o $@
# 
# parallel_util.o : parallel_util.h
# 	$(CXX) $(CXXFLAGS) -I $(PY_LIB) -c $^ -o $@
# 
# xgboost_tree.o : xgboost_tree.h
	$(CXX) $(CXXFLAGS) -I $(PY_LIB) -c $^ -o $@

# g++ -fPIC -shared treemify.cpp -I /System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7/ -o test1.so

.PHONY : clean
clean:
	rm -f *.o
