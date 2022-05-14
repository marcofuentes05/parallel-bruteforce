#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>

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

int tryKey(long key, char *ciph, int len, char* needle){
  char temp[len+1];
  memcpy(temp, ciph, len);
  temp[len]=0;
  DES_Process((char *)key, temp, len, DECRYPTION);
  return strstr((char *)temp, needle) != NULL;
}

int main(int argc, char* argv[]) {
    char *needle = "Lorem";

  FILE *fp;
   char cipher[255];
   fp = fopen("files/lorem.txt", "r");
  fgets(cipher, 255, (FILE*)fp);


  int N, id;
  long upper = (1L <<8); //upper bound DES keys 2^56
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

  long range_per_node = upper / N;
  mylower = range_per_node * id;
  myupper = range_per_node * (id+1) -1;
  if(id == N-1){
    //compensar residuo
    myupper = upper;
  }

  long found = 0;
  int ready = 0;

  MPI_Irecv(&found, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &req);

  for(long i = mylower; i<myupper; ++i){
    MPI_Test(&req, &ready, MPI_STATUS_IGNORE);
    if(ready)
      break;  //ya encontraron, salir

    if(tryKey((char *)i, (char *)cipher, ciphlen, needle)){
      found = i;
      for(int node=0; node<N; node++){
        MPI_Send(&found, 1, MPI_LONG, node, 0, MPI_COMM_WORLD);
      }
      break;
    }
  }

  if(id==0){
    MPI_Wait(&req, &st);
    // decrypt(found, (char *)cipher, ciphlen);
    DES_Process(found, (char *)cipher, ciphlen, DECRYPTION);
    printf("%li %s\n", found, cipher);
  }

  MPI_Finalize();

  double end = MPI_Wtime();

  printf("The process took %f seconds!\n", end - start);
}