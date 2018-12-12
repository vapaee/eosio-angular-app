#include <iostream>
#include <string>
#include <vapaee/slug.hpp>
using namespace std;
using namespace vapaee;

int main ()
{
    slug a = "j";
    // 0F000000000000000000000000000000
    // 0000 1111

    slug b = "jj";
    // 0F00 -> 0000 1111 0000 0000
    // E001 -> 1110 0000 0000 0001  
    // EF010000000000000000000000000000
    // 1110 1111 0000 0001
    // EF01 -> 1110 1111 0000 0001 
    // E03D -> 1110 0000 0011 1101
    
    slug c = "jjj";
    // 21840000000000000000000000000000
    // 0010 0001 1000 0100

    slug d = "jjjj";
    // 42080000000000000000000000000000
    // 0100 0010 0000 1000
    
    cout << "a.to_hexa(): " << a.to_hexa() << "\n";
    cout << "a.to_string(): " << a.to_string() << "\n";
    cout << "b.to_hexa(): " << b.to_hexa() << "\n";
    cout << "b.to_string(): " << b.to_string() << "\n";
    cout << "c.to_hexa(): " << c.to_hexa() << "\n";
    cout << "c.to_string(): " << c.to_string() << "\n";
    cout << "d.to_hexa(): " << d.to_hexa() << "\n";
    cout << "d.to_string(): " << d.to_string() << "\n";
    return 0;
}