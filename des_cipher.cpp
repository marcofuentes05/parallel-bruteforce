#include <iostream>
#include <iomanip>
#include <stdio.h>

#include "cryptopp860/modes.h"
#include "cryptopp860/des.h"
#include "cryptopp860/filters.h"

using namespace CryptoPP;

void DES_Process(const char *keyString, byte *block, size_t length, CryptoPP::CipherDir direction){

    byte key[DES_EDE2::KEYLENGTH];
    memcpy(key, keyString, DES_EDE2::KEYLENGTH);
    BlockTransformation *t = NULL;

    if(direction == ENCRYPTION)
        t = new DES_EDE2_Encryption(key, DES_EDE2::KEYLENGTH);
    else
        t = new DES_EDE2_Decryption(key, DES_EDE2::KEYLENGTH);

    int steps = length / t->BlockSize();
    for(int i=0; i<steps; i++){
        int offset = i * t->BlockSize();
        t->ProcessBlock(block + offset);
    }

    delete t;
}

int main(int argc, char* argv[]) {

    byte block[1024] = "++++++++--------********////////";

    const char *key = "http://qsanguosha.org/forum";

    printf("original text: %s\n", block);

    DES_Process(key, block, 32, CryptoPP::ENCRYPTION);

    printf("Encrypt: %s\n", block);

    DES_Process(key, block, 32, CryptoPP::DECRYPTION);

    printf("Decrypt: %s\n", block);

    return 1;
}
