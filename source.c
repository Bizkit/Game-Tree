#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

const int positive = 1;
const int negative = -1;
const int size = 10;

typedef struct tree_node  {
	char matrix[3][3]; // game board
	struct tree_node **next; // links to child nodes
	int children; // number of child nodes
	int value; // T or F in AND / OR tree
	int size; // size of a tree
} Tree;

// queue of trees
typedef struct queue {
	Tree *tree;
	struct queue *next;
} QueueTree;

// reading game board from file
void reading_matrix (FILE *input, Tree * root) {
	// prima linie luata caracter cu caracter
	fscanf(input, "%c %c %c\n", &root->matrix[0][0], &root->matrix[0][1], &root->matrix[0][2]);
	// a doua linie luata caracter cu caracter
	fscanf(input, "%c %c %c\n", &root->matrix[1][0], &root->matrix[1][1], &root->matrix[1][2]);
	// a treia linie luata caracter cu caracter
	fscanf(input, "%c %c %c", &root->matrix[2][0], &root->matrix[2][1], &root->matrix[2][2]);
}

// checking if game was won by X or 0
int check_winning (Tree *nod, char c) {
	if ((nod->matrix[0][0] == c && nod->matrix[1][1] == c && nod->matrix[2][2] == c)
		|| (nod->matrix[2][0] == c && nod->matrix[1][1] == c && nod->matrix[0][2] == c) // diagonala
		|| (nod->matrix[0][0] == c && nod->matrix[0][1] == c && nod->matrix[0][2] == c)
	 	|| (nod->matrix[1][0] == c && nod->matrix[1][1] == c && nod->matrix[1][2] == c) // linia sau coloana 1
		|| (nod->matrix[2][0] == c && nod->matrix[2][1] == c && nod->matrix[2][2] == c)
		|| (nod->matrix[0][0] == c && nod->matrix[1][0] == c && nod->matrix[2][0] == c) // linia sau coloana 2
		|| (nod->matrix[0][1] == c && nod->matrix[1][1] == c && nod->matrix[2][1] == c)
		|| (nod->matrix[0][2] == c && nod->matrix[1][2] == c && nod->matrix[2][2] == c)) // linia sau coloana 3
		return 1; // player won the game
	return 0; // game didn't end or player lost
}

// checking the empty cells from game board
int matrix_empty_spaces (Tree *node) {
	int n = 0, i, j; // n = number of empty spaces
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (node->matrix[i][j] == '-') // if cell is empty
				n++;
		}
	}
	return n;
}

// checking if a game board if full of X and 0
int check_matrix_full (Tree *node) {
	int n = matrix_empty_spaces(node); // empty spaces
	if (n == 0)
		return 1; // full
	else
		return 0; // not full
}

// print game board of a game tree
// level for indentation
void print_matrix (Tree *node, FILE * output, int level) {
	int i, j, k;
	for (i = 0; i < 3; i++)
	{
		for (k = 0; k < level; k++)
			fprintf(output, "\t");
		for (j = 0; j < 3; j++)
		{
			fprintf(output, "%c", node->matrix[i][j]);
			if (j < 2)
				fprintf(output, " ");
		}
		fprintf(output, "\n");
	}
	// next elementn
	fprintf(output, "\n");
}

// print AND OR tree
void print_tf (Tree *node, FILE * output, int level) {
	int k;
	for (k = 0; k < level; k++)
		fprintf(output, "\t");
	// print T or F
	fprintf(output, "%c", node->value);
	// next element
	fprintf(output, "\n");
	for(k = 0; k < node->children; k++)
		// DFS
		print_tf(node->next[k], output, level + 1);
}

void print_minimax(Tree *node, FILE * output, int level) {
	for (int i = 0; i < level; ++i)
		fprintf(output, "\t");
	fprintf(output, "%d\n", node->value);
	for (int i = 0; i < node->children; ++i)
		print_minimax(node->next[i], output, level + 1);
}

// destroying the tree and its kids
void free_tree (Tree *root) {
	int i;
	for(i = 0; i < root->children; i++)
		free_tree(root->next[i]);
	free(root->next);
	free(root);
}

// game tree element insert
void insert (Tree *tree, char player, int level, FILE* output)
{
	tree->children = matrix_empty_spaces(tree); // number of child nodes
	tree->next = calloc(tree->children, sizeof(Tree*));
	int i, j;
	int index = 0;
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3; j++)
		{
			if(tree->matrix[i][j] == '-')
			{
				Tree *nod = calloc(1, sizeof(Tree));
				memcpy(nod->matrix, tree->matrix, sizeof(tree->matrix));
				nod->matrix[i][j] = player;
				print_matrix(nod, output, level);
				tree->next[index++] = nod;
				if(!check_winning(nod, 'X') && !check_winning(nod, 'O'))
				{
					if(player == 'O')
						insert(nod, 'X', level + 1, output);
					else
						insert(nod, 'O', level + 1, output);
				}
			}
		}
	}
}

// inserting in AND / OR tree
void insert2 (Tree *tree, char player, char max_player, int level, FILE* output) {
	tree->children = matrix_empty_spaces(tree); 
	tree->next = calloc(tree->children, sizeof(Tree*));
	int i, j;
	int index = 0;
	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			if(tree->matrix[i][j] == '-') {
				Tree *nod = calloc(1, sizeof(Tree));
				memcpy(nod->matrix, tree->matrix, sizeof(tree->matrix));
				nod->matrix[i][j] = player;
				tree->next[index++] = nod;
				if(!check_winning(nod, 'X') && !check_winning(nod, 'O')) {
					if(player == 'O')
						insert2(nod, 'X', max_player,level + 1, output);
					else
						insert2(nod, 'O', max_player, level + 1, output);
				}
			}
		}
	}
}

// recursive completion with T and F of AND - OR tree
void tf_matrix (Tree *tree, char player, char max_player, int level, FILE* output) {
	int i;
	if(tree->children == 0) {
		tree->value = 'F';
		if(check_winning(tree, max_player)) {
			tree->value = 'T';
		}
		return;
	}

	tree->value = 'T';
	if (player == max_player) 
		tree->value = 'F';
	for (i = 0; i < tree->children; i++) {
		if(player == 'O')
			tf_matrix(tree->next[i], 'X', max_player, level + 1, output);
		else
			tf_matrix(tree->next[i], 'O', max_player, level + 1, output);
		if (player == max_player) {
			if(tree->next[i]->value == 'T') tree->value = 'T';
		} else {
			if(tree->next[i]->value == 'F') tree->value = 'F';
		}
	}
}

// creating game tree
void game_tree (FILE *input, FILE *output) {
	Tree *tree_game = NULL;
	char player;
	fscanf(input, "%c\n", &player);
	tree_game = calloc(1, sizeof(Tree));
	reading_matrix(input, tree_game);
	print_matrix(tree_game, output, 0);
	insert(tree_game, player, 1, output);
	free_tree(tree_game);
}

// creating AND - OR tree
void and_or_tree (FILE *input, FILE *output) {
	Tree *tree_game = NULL;
	char player; 
	fscanf(input, "%c\n", &player);
	tree_game = calloc(1, sizeof(Tree));
	reading_matrix(input, tree_game); 
	insert2(tree_game, player, player, 1, output);
	tf_matrix(tree_game, player, player, 0, output);
	print_tf(tree_game, output, 0);
	free_tree(tree_game);
}

// using queue for creating minimax tree
QueueTree *pull (QueueTree *queue) {
	if (queue->next == NULL) {
		free(queue);
		return NULL;
	}

	QueueTree *temp = queue;
	queue = queue->next;
	free(temp);
	return queue;
}

void push (QueueTree **queue, Tree *tree) {
	QueueTree *temp = calloc (1, sizeof(QueueTree));
	temp->tree = tree;
	temp->next = NULL;

	if (*queue == NULL) {
		*queue = temp;
		return;
	}

	(*queue)->next = temp;
	*queue = (*queue)->next;
}


int parse (char value) {
	return value - '0';
}

// creating minimax tree, by parsing numbers from input files
Tree* read_minimax (FILE *input, Tree *tree) {
	tree = calloc(1, sizeof(Tree));
	tree->next = calloc(size, sizeof(Tree*));
	tree->size = 10;
	tree->children = 0;
	
	QueueTree *queue_front = NULL;
	QueueTree *queue_end = NULL;
	
	push(&queue_front, tree);
	queue_end = queue_front;

	char character;
	while (fscanf(input, "%c", &character) != EOF) {
		int children = 0, sign;
		if (character == '(') {
			fscanf(input, "%c", &character);
			while (character != ')') {
				children *= 10;
				children += parse(character);
				fscanf(input, "%c", &character);
			}

			queue_front->tree->children = children;
			while (queue_front->tree->size <= size) {
				queue_front->tree->size *= 2;
				int len = queue_front->tree->size;
				queue_front->tree->next = realloc(queue_front->tree->next, 
										len * sizeof(Tree*));
			}
			for (int i = 0; i < children; ++i) {
				queue_front->tree->next[i] = calloc(1, sizeof(Tree));
				queue_front->tree->next[i]->size = size;
				queue_front->tree->next[i]->children = 0;
				queue_front->tree->next[i]->next = calloc(size, sizeof(Tree*));
				push(&queue_end, queue_front->tree->next[i]);
			}
			queue_front = pull(queue_front);
		}

		if (character == '[') {
			fscanf(input, "%c", &character);
			sign = 1;
			int value = 0;
			if (character == '-') {
				fscanf(input, "%c", &character);
				sign = -1;
			}
			while (character != ']') {
				value *= 10;
				value += parse(character);
				fscanf(input, "%c", &character);
			}
			value *= sign;
			queue_front->tree->value = value;
			queue_front = pull(queue_front);

		}
	}
	return tree;
}


// applying minimax algorithm
int minimax_tree (int depth, Tree **tree) {
	if ((*tree)->children > 0) {
		// at least a child for completing the minimax tree
		int temp  = minimax_tree(depth + 1, &((*tree)->next[0]));
		int max = temp, min = temp;

		for (int i = 1; i < (*tree)->children; ++i) {
			temp  = minimax_tree(depth + 1, &((*tree)->next[i]));
			if (max < temp)
				max = temp;
			if (min > temp)
				min = temp;
		}

		if (depth % 2 == 0) {
			(*tree)->value = min;
			return min;
		} else {
			(*tree)->value = max;
			return max;
		}
	} else
		return (*tree)->value;
}

void minimax (FILE *input, FILE *output) {
	Tree *tree = NULL;
	tree = read_minimax(input, tree);
	tree->value = minimax_tree(1, &tree);
	print_minimax(tree, output, 0);
	free_tree(tree);
}
 
// alpha beta pruning optimization
char pruning_tree (int depth, int alpha, int beta, Tree **tree) {
	if ((*tree)->children > 0) {
		if (depth % 2 == 0) {
			int min = INT_MAX, i; // minimum node
			for (i = 1; i <= (*tree)->children; ++i) {
				int temp = pruning_tree(depth + 1, alpha, beta, &((*tree)->next[i - 1]));
				if (min >= temp)
					min = temp;
				if (beta >= min)
					beta = min;
				if (beta <= alpha)
					break;
			}
			if (i + 1 <= (*tree)->children) {
				for (int k = i; k < (*tree)->children; ++k)
					free_tree((*tree)->next[k]);
				(*tree)->children = i;
			}
			(*tree)->value = min;
			return min;
		} else {
			int max = INT_MIN, i; // maximum node
			for (i = 1; i <= (*tree)->children; ++i) {
				int temp = pruning_tree(depth + 1, alpha, beta, &((*tree)->next[i - 1]));
				if (max <= temp)
					max = temp;
				if (alpha <= max)
					alpha = max;
				if (beta <= alpha)
					break;
			}
			if (i + 1 <= (*tree)->children) {
				for (int k = i; k < (*tree)->children; ++k)
					free_tree((*tree)->next[k]);
				(*tree)->children = i;
			}
			(*tree)->value = max;
			return max;
		}
	}
	return (*tree)->value;
}

void pruning (FILE *input, FILE *output) {
	Tree *tree = NULL;
	tree = read_minimax(input, tree);
	tree->value = pruning_tree(1, INT_MIN, INT_MAX, &tree);
	print_minimax(tree, output, 0);
	free_tree(tree);
}

int main (int argc, char **argv)
{
	FILE *file_input = fopen(argv[2], "r"); // deschidem fisierul de intrare
	FILE *file_output = fopen(argv[3], "w"); // deschidem fisierul de iesire
	// showing game tree
	if (strcmp (argv[1], "-c1") == 0)
		game_tree (file_input, file_output);
	// showing AND OR tree
	else if (strcmp (argv[1], "-c2") == 0)
		and_or_tree (file_input, file_output);
	// minimax tree
	else if (strcmp (argv[1], "-c3") == 0) {
		char ignore[10];
		fgets(ignore, 10, file_input);
		minimax (file_input, file_output);
	} else if (strcmp (argv[1], "-b") == 0) { // alpha beta tree
		char ignore[10];
		fgets(ignore, 10, file_input);
		pruning (file_input, file_output);
	}
	// inchidem fisierele
	fclose(file_input);
	fclose(file_output);
	return 0;
}
