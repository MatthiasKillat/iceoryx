#include <iostream>
#include <string>

#include "iceoryx_utils/cxx/string.hpp"

using iox::cxx::string;

int main()
{
    string<10> s1("abc");
    std::string s2("xyz");
    std::cout << s1 << std::endl;
    std::cout << s2 << std::endl;
    return 0;
}