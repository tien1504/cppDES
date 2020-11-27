#include <iostream>
using namespace std;

#include "tests.h"
#include "fileencryption.h"

void usage()
{
    cout << "Usage: cppDES -e/-d key1 key2 key3 input-file output-file" << endl;
}

int main(int argc, char **argv)
{
    //alltests(); return 0;

    if(argc < 7)
    {
        usage();
        return 1;
    }

    string enc_dec = argv[1];
    if(enc_dec != "-e" && enc_dec != "-d")
    {
        usage();
        return 2;
    }

    string input,output;
    if(argc > 5)
        input  = argv[5];
    if(argc > 6)
        output = argv[6];

    ui64 key1 = strtoull(argv[2], nullptr, 16);
    ui64 key2 = strtoull(argv[3], nullptr, 16);
    ui64 key3 = strtoull(argv[4], nullptr, 16);
    FileEncryption fe(key1, key2, key3);
    
    if(enc_dec == "-e")
        return fe.encrypt(input, output);
    if(enc_dec == "-d")
        return fe.decrypt(input, output);

    return 0;
}
