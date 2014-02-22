#include <openssl/sha.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>



struct node_t{
	int size;
	void *data;
	char *hash;
	struct node_t *left;
	struct node_t *right;
};

typedef struct node_t node_t;

char *hashdata(void *n,int size);
node_t *search(node_t *tree,node_t *query);

node_t *new_node(void *data, int size){
	node_t *new;

	new = calloc(1,sizeof(node_t));
	new->hash = (void*) hashdata((void *)data,size);
	new->data = (void*) data;
	new->size = size;
	new->left = NULL;
	new->right = NULL;
//	printf("%s\n",(char*)new->hash);
	return new;

}

int ncmp(node_t *n1,node_t *n2){
	return bcmp(n1->hash,n2->hash,SHA512_DIGEST_LENGTH);
}
void print_tree(node_t *head){
	printf("%s\n",(char*)head->data);
	if(head->left != NULL){
		print_tree(head->left);
	}
	if(head->right != NULL){
		print_tree(head->right);
	}
}

node_t *find_parent(node_t *tree,node_t *query){
	int cmp;
	node_t *current;

	current = tree;
	while(1){
		cmp = ncmp(current,query);
		if(cmp==0){
			fprintf(stderr,"Somehow on top of query node.\n");
			return current;
		}
		if(cmp > 0){
			if(ncmp(current->right,query)==0) break;
			current = current->right;
		}
		if(cmp < 0){
			if(ncmp(current->left,query)==0) break;
			current = current->left;
		}
	}
	return NULL;


}

void free_node(node_t *node){
	free(node->data);
	free(node->hash);
	free(node);
}

void delete(node_t *tree, node_t *offender, node_t *parent){
	node_t *del;


	if((del=search(tree,offender))==NULL){
		fprintf(stderr,"Could not find node to delete.\n");
		return;
	}
	if(del->right==NULL && del->left==NULL){
		if(parent != NULL){
			if(parent->right==del){
				parent->right=NULL;
			}else if(parent->left==del){
				parent->left=NULL;
			}else{
				fprintf(stderr,"Parent isn't actually parent. Odd...\n");
			}
		}
		free_node(del);
		return;
	}
	if(del->right==NULL){
		if(del->left!=NULL){
			del->size = del->left->size;
			del->hash = del->left->hash;
			del->data = del->left->data;
			delete(del->left,del->left,del);
		}
	}else if(del->left==NULL){
		if(del->right!=NULL){
			del->size = del->right->size;
			del->hash = del->right->hash;
			del->data = del->data;
			delete(del->right,del->right,del);
		}
	}else{
			del->size = del->right->size;
			del->hash = del->right->hash;
			del->data = del->data;
			delete(del->right,del->right,del);
		
	}

	

	
	
}

node_t *search(node_t *tree,node_t *query){
	int cmp;
	node_t *current;
	
	current = tree;
	while(1){
		cmp = ncmp(current,query);
		if(cmp==0){
			return current;
		}
		if(cmp > 0){
			if(current->right==NULL) break;
			current = current->right;
		}
		if(cmp < 0){
			if(current->left==NULL) break;
			current = current->left;
		}
	}
	return NULL;
}

int insert_word(node_t **tree,void *data,int size){
	if(*tree==NULL){
		*tree  = new_node(data,size);
		return 1;
	}	
	node_t *new = new_node(data,size);

	node_t *current;

	current = *tree;

	while(1){
		int cmp;
		cmp = ncmp(current,new);
		if(cmp==0){
			return 0;
		}
		if(cmp>0 ){
			if(current->right != NULL){
				current = current->right;
			}else{
				current->right = new;
				return 1;
			}
		}
		if(cmp<0 ){
			if(current->left != NULL){
				current = current->left;
			}else{
				current->left = new;
				return 1;
			}
		}
	}
	fprintf(stderr, "Couldn't insert data but didn't return an error.\nOdd....\n");
	free(new);
	return 0;
}

node_t *load_dictionary(const char *filename){
	FILE *f;
	char *word;
	size_t len;
	int fend;
	node_t *tree=NULL;

	f = fopen(filename,"r");
	fseek(f,0,SEEK_END);
	fend = ftell(f);
	rewind(f);
	while(ftell(f) < fend){
		word=NULL;
		getline(&word,&len,f);
		printf("inserting word: %s\n",word);
		if(insert_word(&tree,word,strlen(word))!=1){
			fprintf(stderr,"DOH! Could not insert word %s\n",word);
		}

	}
	return tree;


}

char *SHA512_hash(const char *data, int len){

	SHA512_CTX ctx;
	char *md;
	if((md=malloc(SHA512_DIGEST_LENGTH))==NULL){
		fprintf(stderr,"Out of memory error...\n");
		exit(1);
	}
	SHA512_Init(&ctx);
	SHA512_Update(&ctx,data,len);
	SHA512_Final((unsigned char *)md,&ctx);
	//md[SHA512_DIGEST_LENGTH] = '\0';
	//md = cleanhash(md,SHA512_DIGEST_LENGTH+1);
	return md;
}

char *hashdata(void *n,int size){
	char message[size];
	char *md;
	memcpy(message, n,size);
	md = SHA512_hash(message,size);
	return md;
}



int main(int argc, char **argv){
	node_t *tree;
	node_t *new;
	char *buffer=NULL;
	size_t t;
	printf("%s\n",hashdata((void *)"asdfasdf",8));
	tree = load_dictionary("random_words.txt");
	printf("Input a word to search for:");
	getline(&buffer,&t,stdin);
	new = new_node((void*)buffer,strlen(buffer));
	//printf("%s\n",(char*) new->hash);
	if(search(tree,new)){
		printf("%s found\n",buffer);
	}
	printf("deleting %s\n",buffer);
	delete(tree,new,NULL);
	if(search(tree,new)){
		printf("%s found\n",buffer);
	}else{
		printf("Did not find %s\n",buffer);
	}
	



	return 0;

}
