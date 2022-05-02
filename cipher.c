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
  char *needle = "Lorem";

  FILE *fp;
   char plain[255];
   fp = fopen("files/lorem.txt", "r");
  fgets(plain, 255, (FILE*)fp);

  printf("FILE CONTENT IS: %s\n", plain);

  int len = strlen(plain); 
  printf("len: %d\n", len);
  char temp[len+1];
  long key = 11;
  memcpy(temp, plain, len);
  temp[len]=0;
  printf("%s\n", temp);
  encrypt(key, temp, len);
  printf("%s\n", temp);
  // decrypt(key, temp, len);
  // printf("%s\n", temp);
  // char *ret = strstr((char *)temp, needle);
  // printf("Occurrence is: %s\n", ret);

  FILE *outFp;
  outFp = fopen("files/ciphered.txt", "w+");
   // fprintf(outFp, "This is testing for fprintf...\n");
   fputs(temp, outFp);
   fclose(outFp);

  if (tryKey(key, temp, len, needle)) {
    printf("CIPHER WORKS!\n");
  }
}
