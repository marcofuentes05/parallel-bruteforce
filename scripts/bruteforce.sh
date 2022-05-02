#!/bin/bash

mpicc -g -Wall -o compiled/bruteforce bruteforce00.c

mpiexec ./compiled/bruteforce
