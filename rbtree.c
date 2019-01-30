#include"rbtree.h"

#define TRUE 1
#define FALSE 0

void init_tree(node **p){
	*p = (node*)malloc(sizeof(node));
	(*p)->left = NULL;
	(*p)->right = NULL;
	(*p)->red = FALSE; 
}

node* rb_search(char* key, node* base){
	node *n;
	n = base->left;

	while(n != NULL && strcmp(key, n->val.key)){
		if(strcmp(key, n->val.key) < 0) n = n->left;
		else n = n->right;
	}

	return n;
}

void show_list(node* table){
	if(table != NULL){
		show_list(table->left);
		printf("%d\n", table->val.hit);
		show_list(table->right);
	}else return ;
}

node* rotate(char* key, node* pivot, node* base){
	node *child, *gchild;
	
	child = (strcmp(key, pivot->val.key) < 0 || pivot == base) ? pivot->left : pivot->right;
	if(strcmp(key, child->val.key) < 0){ 
		gchild = child->left;
		child->left = gchild->right;
		gchild->right = child;
	}else{
		gchild = child->right;
		child->right = gchild->left;
		gchild->left = child;
	}
	
	if(strcmp(key, pivot->val.key) < 0 || pivot == base)
		pivot->left = gchild;
	else
		pivot->right = gchild;
	
	return gchild;
}

node* rb_insert(data info, node* base){
	node *t, *parent, *gparent, *ggparent; 
	ggparent = gparent = parent = base;
	
	t = base->left; 
	
	while(t != NULL){
		if(!strcmp(info.key, t->val.key)) return NULL;
		if(t->left && t->right && t->left->red && t->right->red){ 
			t->red = TRUE;
			t->left->red = t->right->red = FALSE;
			if(parent->red){ 
				gparent->red = TRUE;  
				
				if(strcmp(info.key, gparent->val.key) != strcmp(info.key, parent->val.key)) 
					parent = rotate(info.key, gparent, base);
				
				t = rotate(info.key, ggparent, base);
				t->red = FALSE;
			}
			base->left->red = FALSE; 
		}
		ggparent = gparent;
		gparent = parent;
		parent = t;
			
		t = (strcmp(info.key, t->val.key) < 0) ? t->left : t->right;
	}
	
	if( (t = (node*)malloc(sizeof(node))) == NULL ) 
		return NULL;
	
	t->val = info;
	t->left = NULL;
	t->right = NULL;
	if(parent == base || strcmp(info.key, parent->val.key) < 0) 
		parent->left = t;
	else
		parent->right = t;
		
	rb_cnt++;  
	t->red = TRUE; 
	
	if(parent->red){
		gparent->red = TRUE;
		if(strcmp(info.key, gparent->val.key) != strcmp(info.key, parent->val.key))
			parent = rotate(info.key, gparent, base);
		
		t = rotate(info.key, ggparent, base);
		t->red = FALSE;
	}
	
	base->left->red = FALSE;
	return t;
}

