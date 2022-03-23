/****************************************************************************
 *
 * Function:	sort
 *
 * Description:	Routines to perform sorting functions
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <string.h>
#include "global_defines.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include "global_debug.h" // Mazieri's debug facilities

/* alloc.c stuff */
tree_node *new_tree_node()
{
    DBG();
static tree_node real_tree_node;
tree_node *tree_nodeptr;

	tree_nodeptr = (tree_node *)malloc((unsigned)sizeof(real_tree_node));
	tree_nodeptr->key = NULL;
	tree_nodeptr->balance = 0;
	tree_nodeptr->more = NULL;
	tree_nodeptr->less = NULL;
	return((tree_node *)tree_nodeptr);
};

/* end of alloc.c stuff */

/* frees the storage of the tree as it wals down the tree */
/* Puts file names in global array files */
void free_bin_tree(tree_top, files, cnt)
tree_node **tree_top;
    char *files[];
    int *cnt;
{
    DBG();

	if((*tree_top)->less != NULL) free_bin_tree(&((*tree_top)->less), files, cnt); 
	/* Free old memory and allocate new memory */
    free(files[*cnt]);
    files[*cnt] = malloc(strlen((*tree_top)->key)+1);
	strcpy(files[*cnt], (*tree_top)->key);
    ++*cnt;
	if((*tree_top)->more != NULL) free_bin_tree(&((*tree_top)->more), files, cnt);
	if((*tree_top)->key != NULL) free((*tree_top)->key);
	free(*tree_top);
	*tree_top = NULL;
} /* End free_bin_tree */

void write_bin_tree(fp, tree_top)
    FILE *fp;
tree_node **tree_top;
{
    DBG();

	if((*tree_top)->less != NULL) write_bin_tree(fp, &((*tree_top)->less));
	fprintf(fp, "%s\n",(*tree_top)->key);
	if((*tree_top)->more != NULL) write_bin_tree(fp, &((*tree_top)->more));
	if((*tree_top)->key != NULL) free((*tree_top)->key);
	free(*tree_top);
	*tree_top = NULL;
} /* End write_bin_tree */

/* Adapted 5/9/1990 for NXP from page 220 of */
/*"Algorithms + Data Structures = Programs" by Niklaus Wirth */
int add_node(tree_top,key,h)
tree_node **tree_top;
char *key;
int *h;
{
    DBG();

tree_node *tree_ptr;
tree_node *tree_ptr2;
tree_node **tree_ptrptr;


	/* is this the first bf.entry */
	if(*tree_top == NULL){
		/* yes this is the first bf.entry */
		tree_ptr = new_tree_node();
		tree_ptr->key = (char *)(malloc((unsigned)strlen(key) +1));
		strcpy(tree_ptr->key,key);
		tree_ptr->balance = 0;
		*tree_top = tree_ptr;
		*h = 1;
	}else{
		/* no this is not the first bf.entry */
		tree_ptrptr = tree_top;

		/* find the place we add this thing to the tree */
		if(strcmp((*tree_ptrptr)->key,key) > 0){
			add_node(&((*tree_ptrptr)->less),key,h);
			/* did the less side of this tree grow */
			if(*h){
				switch((*tree_ptrptr)->balance){
					case 1:
						(*tree_ptrptr)->balance = 0;
						*h = 0;
						break;
					case 0:
						(*tree_ptrptr)->balance = -1;
						*h = 1;
						break;
					case -1: /* rebalance */
						tree_ptr = (tree_node *)(*tree_ptrptr)->less;
						if(tree_ptr->balance == -1){
							/* single LL rotation */
 							/*printf("do single LL rotation \n");*/
							(*tree_ptrptr)->less = tree_ptr->more;
							tree_ptr->more = (struct tree_node *)*tree_ptrptr;
							(*tree_ptrptr)->balance = 0;
							*tree_ptrptr = tree_ptr;
						}else{
 							/*printf("do double LR rotation \n");*/
							tree_ptr2 = (tree_node *)tree_ptr->more;
							tree_ptr->more = tree_ptr2->less;
							tree_ptr2->less = (struct tree_node *)tree_ptr;
							(*tree_ptrptr)->less = tree_ptr2->more;
							tree_ptr2->more = (struct tree_node *)(*tree_ptrptr);
							if(tree_ptr2->balance == -1)
								(*tree_ptrptr)->balance = 1;
							else
								(*tree_ptrptr)->balance = 0;

							if(tree_ptr2->balance == 1)
								tree_ptr->balance = -1;
							else
								tree_ptr->balance = 0;
							*tree_ptrptr = tree_ptr2;
						}
						(*tree_ptrptr)->balance = 0;
						*h = 0;
						break;
				}
			}
		}else if(strcmp((*tree_ptrptr)->key,key) < 0){
			add_node(&((*tree_ptrptr)->more),key,h);
			/* did the more side of this tree grow */
			if(*h){
				switch((*tree_ptrptr)->balance){
					case -1:
						(*tree_ptrptr)->balance = 0;
						*h = 0;
						break;
					case 0:
						(*tree_ptrptr)->balance = 1;
						*h = 1;
						break;
					case 1: /* rebalance */
						tree_ptr = (tree_node *)(*tree_ptrptr)->more;
						if(tree_ptr->balance == 1){
							/* single RR rotation */
 							/*printf("do single RR rotation \n");*/
							(*tree_ptrptr)->more = (struct tree_node *)tree_ptr->less;
							tree_ptr->less = (struct tree_node *)(*tree_ptrptr);
							(*tree_ptrptr)->balance = 0;
							*tree_ptrptr = tree_ptr;
						}else{
 							/*printf("do double RL rotation \n");*/
							tree_ptr2 = (tree_node *)tree_ptr->less;
							tree_ptr->less = tree_ptr2->more;
							tree_ptr2->more = (struct tree_node *)tree_ptr;
							(*tree_ptrptr)->more = tree_ptr2->less;
							tree_ptr2->less = (struct tree_node *)(*tree_ptrptr);
							if(tree_ptr2->balance == 1)
								(*tree_ptrptr)->balance = -1;
							else
								(*tree_ptrptr)->balance = 0;

							if(tree_ptr2->balance == -1)
								tree_ptr->balance = 1;
							else
								tree_ptr->balance = 0;
							*tree_ptrptr = tree_ptr2;
						}
						(*tree_ptrptr)->balance = 0;
						*h = 0;
						break;
				}
			}
		}else{
			/* they must be equal, duplicate key found */
			/* the node which has the same key as the node we are */
			/* trying to add, is pointed to by (*tree_ptrptr) */
			/* an application might require that a change to the data */
			/* currently stored in the location be made.  Here we simply */
			/* print out an error message and return */
			printf("Duplicate binary key found, not legal\n");
			return(0);
		}
	}
	return(1);
}


void balance1(p,h)
tree_node **p;
int *h;
{
    DBG();

tree_node *p1,*p2;
int b1,b2;

	switch((*p)->balance){
		case -1:
			(*p)->balance = 0;
			break;
		case 0:
			(*p)->balance = 1;
			*h = 0;
			break;
		case 1:
			p1 = (tree_node *)(*p)->more;
			b1 = p1->balance;
			if(b1 >= 0){
				/* single RR rotation */
				/*printf("do single RR rotation \n");*/
				(*p)->more = p1->less;
				p1->less = (struct tree_node *)(*p);
				if(b1 == 0){
					(*p)->balance = 1;
					p1->balance = -1;
					*h = 0;
				}else{
					(*p)->balance = 0;
					p1->balance = 0;
				}
				*p = p1;
			}else{
				/* double RL rotation */
				/*printf("do double RL rotation \n");*/
				p2 = (tree_node *)p1->less;
				b2 = p2->balance;
				p1->less = p2->more;
				p2->more = (struct tree_node *)p1;
				(*p)->more = p2->less;
				p2->less = (struct tree_node *)(*p);
				if(b2 == 1){
					(*p)->balance = -1;
				}else{
					(*p)->balance = 0;
				}
				if(b2 == -1){
					p1->balance = 1;
				}else{
					p1->balance = 0;
				}
				*p = p2;
				p2->balance =0;
			}
			break;
	}
}
void balance2(p,h)
tree_node **p;
int *h;
{
    DBG();

tree_node *p1,*p2;
int b1,b2;

	switch((*p)->balance){
		case 1:
			(*p)->balance = 0;
			break;
		case 0:
			(*p)->balance = -1;
			*h = 0;
			break;
		case -1:
			p1 = (tree_node *)(*p)->less;
			b1 = p1->balance;
			if(b1 <= 0){
				/* single LL rotation */
				/*printf("do single LL rotation \n");*/
				(*p)->less = p1->more;
				p1->more = (struct tree_node *)(*p);
				if(b1 == 0){
					(*p)->balance = -1;
					p1->balance = 1;
					*h = 0;
				}else{
					(*p)->balance = 0;
					p1->balance = 0;
				}
				*p = p1;
			}else{
				/* double LR rotation */
				/*printf("do double LR rotation \n");*/
				p2 = (tree_node *)p1->more;
				b2 = p2->balance;
				p1->more = p2->less;
				p2->less = (struct tree_node *)p1;
				(*p)->less = p2->more;
				p2->more = (struct tree_node *)(*p);
				if(b2 == -1){
					(*p)->balance = 1;
				}else{
					(*p)->balance = 0;
				}
				if(b2 == 1){
					p1->balance = -1;
				}else{
					p1->balance = 0;
				}
				*p = p2;
				p2->balance =0;
			}
			break;
	}
}
void del(r,h,q)
tree_node **r;
int *h;
tree_node *q;
{
    DBG();

	if((*r)->more != NULL){
		del(&((*r)->more),h,q);
		if(*h){
			balance2(r,h);
		}
	}else{
		q->key = (*r)->key;
		*r = (tree_node *)(*r)->less;
		*h = 1;
	}
}
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
