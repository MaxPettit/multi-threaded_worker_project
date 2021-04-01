#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

#define ARRAY_SIZE 10
#define MAX_NAME_LENGTH 255

void array_init(void);
void array_push(char * hostname);
void array_pop(char * readname);

#endif