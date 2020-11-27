#ifndef FILEENCRYPTION_H
#define FILEENCRYPTION_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//#include "descbc.h"
#include "des3.h"

class FileEncryption
{
public:
    FileEncryption(ui64 key1, ui64 key2, ui64 key3);
    int encrypt(string input, string output);
    int decrypt(string input, string output);
    int cipher (string input, string output, bool mode);

private:
    //DESCBC des;
    DES3 des; // use DES3 instead
};

#endif // FILEENCRYPTION_H
