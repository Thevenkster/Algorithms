/***
compile and run (in linux):
g++ sha256.cpp main.cpp -o sha256_example && ./sha256_example
Output:
sha256('grape'): 0f78fcc486f5315418fbf095e71c0675ee07d318e5ac4d150050cd8e57966496
**/

#include <iostream>
#include "sha256.h"

using std::string;
using std::cout;
using std::endl;
 
int main(int argc, char *argv[])
{
    string input = "grape    sha256('grape'):0f78fcc486f5315418fbf095e71c0675ee07d318e5ac4d150050cd8e57966496";
    string output1 = sha256(input);
 
    cout << "sha256('"<< input << "'):" << output1 << endl;
    return 0;
}