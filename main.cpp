#include <iostream>
#include <cassert>

#include "xor-list.h"

template<typename V> std::ostream& operator<<(std::ostream& out, const xor_list<V>& lst) {
    const char* prefix = "";
    for (const int& v : lst) {
        out << prefix << v;
        prefix = ", ";
    }
    return out;
}

int main() {
    xor_list<int> lst;
    lst.push_back(10);
    lst.push_back(12);
    lst.push_front(8);
    assert(lst == (xor_list<int> { 8, 10, 12 }));

    assert(!!lst);
    lst.erase(lst.begin(), lst.end());
    assert(!lst);

    lst = xor_list<int> { 2, 3, 5, 7 };
    assert(lst == (xor_list<int> { 2, 3, 5, 7 }));
    lst.erase(lst.begin(), ++lst.begin());
    assert(lst == (xor_list<int> { 3, 5, 7 }));
    lst.erase(lst.rbegin(), ++lst.rbegin());
    assert(lst == (xor_list<int> { 3, 5 }));
    lst = xor_list<int> { 2, 3, 5, 7 };
    lst.erase(++lst.begin(), ++ ++ ++lst.begin());
    assert(lst == (xor_list<int> { 2, 7 }));

    lst.erase(lst.begin(), lst.begin());
    lst.erase(lst.rbegin(), lst.rbegin());
    lst.erase(++lst.begin(), ++lst.begin());
    assert(lst == (xor_list<int> { 2, 7 }));

    return 0;
}
