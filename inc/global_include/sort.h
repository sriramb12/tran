typedef struct {
	char * key;         /* the key used in the comparison */
	int balance;			/* info about the state of the tree at this node */
							/* 0 if balanced, -1 if heavy to less, 1 if heavy to more */
	struct tree_node *less;
	struct tree_node *more;
}tree_node;

