#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "des_crypt.h"

void decrypt(long key, char *ciph, int len){
  //set parity of key and do decrypt
  long k = 0;
  for(int i=0; i<8; ++i){
    key <<= 1;
    k += (key & (0xFE << i*8));
  }
  des_setparity((char *)&k);  //el poder del casteo y &
  ecb_crypt((char *)&k, (char *) ciph, 16, DES_DECRYPT);
}

void encrypt(long key, char *ciph, int len){
  //set parity of key and do decrypt
  long k = 0;
  for(int i=0; i<8; ++i){
    key <<= 1;
    k += (key & (0xFE << i*8));
  }
  des_setparity((char *)&k);  //el poder del casteo y &
  ecb_crypt((char *)&k, (char *) ciph, 16, DES_ENCRYPT);
}

// char search[] = "testt";

int tryKey(long key, char *ciph, int len, char* search){
  char temp[len+1];
  memcpy(temp, ciph, len);
  temp[len]=0;
  decrypt(key, temp, len);
  return strstr((char *)temp, search) != NULL;
}


int main(int argc, char * argv[]) {
  char *plain = "testt";
  char *needle = "tt";
  int len = 5; 
  char temp[len+1];
  long key = 555;
  memcpy(temp, plain, len);
  temp[len]=0;
  printf("%s\n", temp);
  encrypt(key, temp, len);
  printf("%s\n", temp);
  // decrypt(key, temp, len);
  // printf("%s\n", temp);
  // char *ret = strstr((char *)temp, needle);
  // printf("Occurrence is: %s\n", ret);
  if (tryKey(key, temp, len, needle)) {
    printf("CIPHER WORKS!\n");
  }
}