//bruteforce.c
//nota: el key usado es bastante pequenio, cuando sea random speedup variara


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "des_crypt.h"

void decrypt(long key, char *ciph, int len){
  long k = 0;
  for(int i=0; i<8; ++i){
    key <<= 1;
    k += (key & (0xFE << i*8));
  }
  des_setparity((char *)&k);
  ecb_crypt((char *)&k, (char *) ciph, len, DES_DECRYPT);
}

void encrypt(long key, char *ciph, int len){
  long k = 0;
  for(int i=0; i<8; ++i){
    key <<= 1;
    k += (key & (0xFE << i*8));
  }
  des_setparity((char *)&k);
  ecb_crypt((char *)&k, (char *) ciph, len, DES_ENCRYPT);
}

int tryKey(long key, char *ciph, int len, char* needle){
  char temp[len+1];
  memcpy(temp, ciph, len);
  temp[len]=0;
  decrypt(key, temp, len);
  return strstr((char *)temp, needle) != NULL;
}

unsigned char cipher[] = {108, 245, 65, 63, 125, 200, 150, 66, 17, 170, 207, 170, 34, 31, 70, 215, 0};

int main(int argc, char *argv[]){
  printf("STARTED EXECUTION!\n");
  char *needle = " the ";
  clock_t t;
  t = clock();

  printf("cipher: %s\n", cipher);

  long upper = (1L <<32);
  printf("UPPER=%ld\n", upper);
  int ciphlen = strlen(cipher);
  printf("ciphlen:%d\n", ciphlen);
  printf("cipher:%s\n", cipher);

  long found = 0;

  for(long i = 0; i<upper; i++){

    if (i == 55 ){
      printf("55\n");

      char temp[ciphlen+1];
      memcpy(temp, cipher, ciphlen);
      temp[ciphlen]=0;
      decrypt(55, temp, ciphlen);
      printf("%s\n", temp);
      printf("%s\n", cipher);
    }
    if(tryKey(i, (char *)cipher, ciphlen, needle)){
      printf("FOUNDED!\n");
      printf("i=%ld\n", i);
      found = i;
      break;
    }
  }
  printf("Found is: %ld\n", found);
  decrypt(found, (char *)cipher, ciphlen);

  printf("Plain is: %s\n", cipher);

   t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
  
    printf("Process took %f seconds to execute \n", time_taken);
}
