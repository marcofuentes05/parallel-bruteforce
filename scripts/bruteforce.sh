#!/bin/bash

mpicc -g -Wall -o compiled/bruteforce bruteforce00.c

mpiexec -n 2 ./compiled/bruteforce