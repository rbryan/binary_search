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

node_t *new_node(void *data, int size){
	node_t *new;

	new = calloc(1,sizeof(node_t));
	new->hash = (void*) hashdata((void *)data,size);
	new->data = (void*) data;
	new->size = size;
	new->left = NULL;
	new->right = NULL;
	printf("%s\n",(char*)new->hash);
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

int delete(node_t *tree, node_t *offender){
	int cmp;
	node_t *current;
	node_t *del;
	current = tree;
	int flag;

	if(search(tree,offender)==0){
		fprintf(stderr,"Could not find node to delete.\n");
		return 0;
	}
	while(1){
		cmp = ncmp(current,offender);
		if(cmp==0){
			break;
		}
		if(cmp > 0){
			if(ncmp(current->right,offender)==0){
				flag = 1;      	
				break;
			}
			current = current->right;
		}
		if(cmp < 0){
			if(ncmp(current->left,offender)==0){
				flag = 0;      	
				break;
			}
			current = current->left;
		}
	}
	if(flag==0)del=current->left;
	if(flag==1)del=current->right;
	if(del->left==NULL){
		
	}
	
}

int search(node_t *tree,node_t *query){
	int cmp;
	node_t *current;
	
	current = tree;
	while(1){
		cmp = ncmp(current,query);
		if(cmp==0){
			return 1;
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
	return 0;
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
	getline(&buffer,&t,stdin);
	new = new_node((void*)buffer,strlen(buffer));
	printf("%s\n",(char*) new->hash);
	printf("%d\n",search(tree,new));



	return 0;

}
