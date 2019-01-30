#include"hash.h"

void init_hash(slot a[]){
	int i;
	for(i = 0 ; i < HASH_SIZE ; ++i) 
		a[i].next = NULL;
}

int hash_func(char key[]){
	return key[5] ^ (key[4] + key[5] * (key[6]>>2) + (key[4]<<3)) % HASH_SIZE; 
}

slot* hash_insert(data info, slot a[]){
	int hash;
	slot *t;

	t = (slot*)malloc(sizeof(slot));
	hash = hash_func(info.key);
	t->next = a[hash].next;
	
	strcpy(t->key, info.key);
	strcpy(t->record, info.record);
	t->hit = info.hit;
	
	a[hash].next = t;
	hash_cnt++;
	
	return t;
}

slot* hash_search(char* key, slot a[]){
	slot *t;
	
	t = a[hash_func(key)].next;
	while(t != NULL && strcmp(t->key, key))
		t = t->next;
	
	return t;
}


