#include <iostream>
#include <string>
#include <vapaee/slug.hpp>
using namespace std;
using namespace vapaee;

int main ()
{
    slug a = "abc";
    
    cout << "a.to_string(): " << a.to_string() << "\n";
    cout << "sizeof(slug): " << sizeof(slug) << "\n";
    cout << "sizeof(int): " << sizeof(int) << "\n";
    cout << "sizeof(long): " << sizeof(long) << "\n";
    cout << "sizeof(long long): " << sizeof(long long) << "\n";
    cout << "sizeof(double): " << sizeof(double) << "\n";
    return 0;
}