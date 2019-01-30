#ifndef _HASH_H_
#define _HASH_H_

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"rbtree.h"

#define HASH_SIZE 5000

int hash_cnt;
typedef struct _slot{
	int hit;
	char key[KEY_SIZE];
	char record[RECORD_SIZE];
	struct _slot* next;
}slot;

int hash_func(char key[]);
slot* hash_insert(data info, slot a[]);
slot* hash_search(char* key, slot a[]);
void init_hash(slot a[]);

#endif
