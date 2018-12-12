#include <iostream>
#include <string>
#include <vapaee/slug.hpp>
using namespace std;
using namespace vapaee;

int main ()
{
    slug a = "abcdefghijklmnopqrstuvwxyz.12345";
    slug b = "1zzzzzzzz21zzzzzzzz21zzzzzzzz21zzzzzzzz21zzzzzzzz2";

    cout << "a.to_hexa(): " << a.to_hexa() << "\n";
    cout << "b.to_hexa(): " << b.to_hexa() << "\n";
    /*
    slug b = "z";
    
    slug c = "jjj";

    slug d = "jjjj";
    
    cout << "a.to_hexa(): " << a.to_hexa() << "\n";
    cout << "a.to_string(): " << a.to_string() << "\n";
    cout << "b.to_hexa(): " << b.to_hexa() << "\n";
    cout << "b.to_string(): " << b.to_string() << "\n";
    cout << "c.to_hexa(): " << c.to_hexa() << "\n";
    cout << "c.to_string(): " << c.to_string() << "\n";
    cout << "d.to_hexa(): " << d.to_hexa() << "\n";
    cout << "d.to_string(): " << d.to_string() << "\n";
    */

    cout << "a.to_string(): " << a.to_string() << "\n";
    cout << "b.to_string(): " << b.to_string() << "\n";

    return 0;
}