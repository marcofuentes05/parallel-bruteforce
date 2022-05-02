//bruteforce.c
//desarrollado a partir de la teoria/snippets de Barlas, 5.21:
//nota: el key usado es bastante pequenio, cuando sea random speedup variara

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
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
  ecb_crypt((char *)&k, (char *) ciph, 256, DES_DECRYPT);
}

void encrypt(long key, char *ciph, int len){
  //set parity of key and do decrypt
  long k = 0;
  for(int i=0; i<8; ++i){
    key <<= 1;
    k += (key & (0xFE << i*8));
  }
  des_setparity((char *)&k);  //el poder del casteo y &
  ecb_crypt((char *)&k, (char *) ciph, 256, DES_ENCRYPT);
}

// char search[] = " the ";
int tryKey(long key, char *ciph, int len, char* needle){
  char temp[len+1];
  memcpy(temp, ciph, len);
  temp[len]=0;
  decrypt(key, temp, len);
  return strstr((char *)temp, needle) != NULL;
}

unsigned char cipher[] = {108, 245, 65, 63, 125, 200, 150, 66, 17, 170, 207, 170, 34, 31, 70, 215, 0};
int main(int argc, char *argv[]){ //char **argv

  char *needle = "Lorem";

  FILE *fp;
   char cipher[255];
   fp = fopen("files/lorem.txt", "r");
  fgets(cipher, 255, (FILE*)fp);


  int N, id;
  long upper = (1L <<56); //upper bound DES keys 2^56
  long mylower, myupper;
  MPI_Status st;
  MPI_Request req;
  int flag;
  int ciphlen = strlen(cipher);
  MPI_Comm comm = MPI_COMM_WORLD;

  double start = MPI_Wtime();

  MPI_Init(NULL, NULL);
  MPI_Comm_size(comm, &N);
  MPI_Comm_rank(comm, &id);

  int range_per_node = upper / N;
  mylower = range_per_node * id;
  myupper = range_per_node * (id+1) -1;
  if(id == N-1){
    //compensar residuo
    myupper = upper;
  }

  long found = 0;

  //non blocking receive, revisar en el for si no es 0
  MPI_Irecv(&found, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &req);

  for(int i = mylower; i<myupper && (found==0); ++i){
    if(tryKey(i, (char *)cipher, ciphlen, needle)){
      found = i;
      for(int node=0; node<N; node++){
        MPI_Send(&found, 1, MPI_LONG, node, 0, MPI_COMM_WORLD);
      }
      break;
    }
  }

  if(id==0){
    MPI_Wait(&req, &st);
    decrypt(found, (char *)cipher, ciphlen);
    printf("\n");
    printf("%li %s\n", found, cipher);
  }

  MPI_Finalize();

  double end = MPI_Wtime();

  printf("The process took %f seconds!\n", end - start);
}
