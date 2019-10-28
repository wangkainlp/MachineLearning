
#include <iostream>
#include <vector>
using namespace std;

void fun(const string& str) {
    cout << str << endl;
}

typedef struct T {
    int value = 0;
} T;

void fun1(string str) {
    cout << str << endl;
}



class Node {
public:
    Node() {
        cout << "构造函数" << endl;
        value = 0;
    }
    Node(int v) {
        cout << "构造函数 param" << endl;
        value = v;
    }
    Node(const Node& node) {
        cout << "拷贝构造函数" << endl;
        if (this != &node) {
            value = node.value;
        }
    }
    Node& operator=(const Node& node) {
        cout << "拷贝赋值运算符" << endl;
        if (this != &node) {
            value = node.value;
        }
        return *this;
    }

    Node(Node&& node) {
        cout << "移动构造函数" << endl;
        if (this != &node) {
            value = node.value;
        }
    }

    Node& operator=(Node&& node) {
        cout << "移动赋值运算符" << endl;
        if (this != &node) {
            value = node.value;
        }
        return *this;
    }

    int value;
};


Node construct() {
    return Node(-1);
}

int main() {
    static char s[10];
    s[0] = 'a';
    s[9] = '\0';
    auto f = []() -> const string{ return std::string(s); };
    fun( f() );


    string s1 = string(s);
    cout << s1 << endl;
    fun1(string(s));

    vector<string> vec1;
    vec1.push_back(string("a"));
    vec1.push_back(string("b"));
    vec1.push_back(string("c"));

    vector<string> vec2 = vec1;
    vec1[0] = string("aa");
    cout << "print" << endl;
    for (auto it : vec2) {
        cout << it << endl;
    }

    vector<T*> vect;
    T t1;
    vect.push_back(&t1);
    t1.value = 1;
    for (auto& it : vect) {
        cout << it->value << endl;
    }

    
    cout << "**********************************************" << endl;
    vector<int> vec_1;
    vec_1.push_back(0);
    vec_1.push_back(1);

    vec_1.back() = 11;
    vec_1.front() = 10;

    for (auto& it : vec_1) {
        cout << it << endl;
    }

    cout << "**********************************************" << endl;

    Node n1;
    n1.value = 10;

    Node n2(n1);
    cout << n2.value << endl;

    Node n3(move(n1));
    cout << n3.value << endl;

    Node& n5 = n1;
    cout << n5.value << endl;
    
    Node&& n7 = construct();
    Node n6;
    swap(n6, n7);
    cout << n6.value << endl;
    cout << n7.value << endl;

    cout << "**********************************************" << endl;

    Node n11;
    Node n12;
    vector<Node> node_vec;
    node_vec.push_back(n11);

    node_vec.emplace_back(n12);
    
    cout << "**********************************************" << endl;
    vector<vector<int>> vecs;
    vector<int> _vec1;
    _vec1.push_back(1);
    _vec1.push_back(2);
    vecs.push_back(_vec1);

    vector<int> _vec2;
    _vec2.push_back(11);
    _vec2.push_back(12);
    vecs.push_back(_vec2);

    cout <<  vecs[0].back() << endl;

    
}
