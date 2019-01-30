#ifndef _RBTREE_H_
#define _RBTREE_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define TRUE 1
#define FALSE 0
#define KEY_SIZE 128
#define RECORD_SIZE 512

int rb_cnt;
typedef struct _data{
	char key[KEY_SIZE];
	char record[RECORD_SIZE];
	int hit;
}data;

typedef struct _node{
	struct _node *left;
	struct _node *right;
	data val;	
	int red;
}node;

void init_tree(node** p);
node* rb_search(char* key, node* base);
node* rotate(char* key, node* pivot, node* base);
node* rb_insert(data info, node* base);
void show_list(node* table);

#endif
