#include <iostream>
#include <string>

#include "iceoryx_utils/cxx/string.hpp"
#include "storage.hpp"
#include "stringref.hpp"

using iox::cxx::string;

void func1(const string<10>& s)
{
    std::cout << "func1 " << s << std::endl;
}

// could also be used as value object (will be created if needed when used by reference)
void func2(const stringref& r)
{
    std::cout << "func2 " << r << " capacity " << r.capacity() << " size " << r.size() << std::endl;
    auto s = r.to_string<100>();
}

void compare(const stringref& a, const stringref& b)
{
    std::cout << a << "==" << b << ":" << (a == b) << std::endl;
    std::cout << a << "!=" << b << ":" << (a != b) << std::endl;
}

void assign(stringref lhs, const stringref& rhs)
{
    std::cout << "assign " << rhs << " to " << lhs;
    lhs = rhs;
    std::cout << " result " << lhs << std::endl;
}

void modify(stringref r)
{
    std::cout << r << std::endl;
    r[1] = 'M';
    std::cout << r << std::endl;
}

int main()
{
    string<10> s1("cxx");
    std::string s2("std");
    string<39> s3("mat");

    std::string s4("cxx");
    std::cout << s1 << std::endl;
    std::cout << s2 << std::endl;
    func1(s1);
    // func1(s3); //impossible

    func2(s1);
    func2(s2);
    func2(s3);
    func2("const char*");

    compare(s1, s1);
    compare(s1, s2);
    compare(s2, s2);
    compare(s1, s4);
    compare("foo", "foo");
    compare("cxx", s1);
    compare("foo", s1);

    assign(s1, "foo");
    std::cout << s1 << std::endl;
    assign(s1, "1234567890x");

    assign(s2, "1234567890x");
    std::cout << s2 << std::endl;
    assign("const", "bar");

    modify(s1);
    modify(s2);

    stringref ref(s1);
    auto stdStr = ref.to_string();

    storage<32> buffer;
    auto ptr1 = buffer.allocate<string<10>>();
    ptr1 = buffer.allocate<string<10>>();
    buffer.deallocate();
    auto ptr2 = buffer.allocate<string<16>>();
    buffer.deallocate();
    auto ptr3 = buffer.allocate<string<32>>();
}