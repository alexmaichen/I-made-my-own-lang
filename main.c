/*
loosely inspired by the following Computerphile video: https://www.youtube.com/watch?v=Q2UDHY5as90
*/

// pick the program you wish to execute (or write your own below)
#define PROG_PRESET 0

#if PROG_PRESET == 0
#define PROG ""
// -> 0 because empty tree AKA nullpointer gets interpreted as 0
#endif

#if PROG_PRESET == 1
#define PROG "1 this is an accidental comment"
// -> 1 because trailing elements do not matter. this means comments are simply done by having more numbers than necessary, at which point the interpreter will have already stopped
#endif

#if PROG_PRESET == 2
#define PROG "* + 12"
// -> empty subtrees AKA nullpointers get interpreted as 0
#endif

#if PROG_PRESET == 3
#define PROG "/ * + 1 - 4 2 3 3"
// -> 3 nesting instuctions allows for more complex behavior
#endif

#if PROG_PRESET == 4
#define PROG "+ 2 'this is a comment' 1"
// -> 1
#endif

#if PROG_PRESET == 5
#define PROG "* 14 ? !1\n+ 0 1 - 0 1"
// -> -14 we have negative numbers btw :D
#endif

#if PROG_PRESET == 6
#define PROG "= A 6 e      # A"
// -> 6 (and A == 6, which will like all variables get printed out at the end of the programs execution)
#endif

#if PROG_PRESET == 7
#define PROG "(^ 3 3)"
// -> 27 parentheses may be added for readability, but not mandatory. The parser just skips over them all the same.
#endif

#define BUFFER_SIZE 256
#define ALPHABET_SIZE ('Z' - 'A' + 1)
#define IS_EOF (**expression == '\0' || **expression == EOF)
#define SKIP_SPACES while(**expression == ' ' || **expression == '\n' || **expression == '(' || **expression == ')') (*expression)++;
#define SKIP_COMMENT if(isComment(**expression)){do{(*expression)++;} while(!isComment(**expression) && **expression != '\0' && **expression != EOF); if(!IS_EOF) (*expression)++;}
#define SKIP_UNNECESSARY SKIP_SPACES SKIP_COMMENT
#define EXECUTABLE_NAME "myCode"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

enum{
	NoError,
	ParserError, // thought it would be necessary, but became unused
	ExecError,
	FileOpenError,
	MiscError
}; // errorcodes

typedef enum{
	OPERATOR,
	OPERAND,
	LOOP,
	VARIABLE
} NodeType; // types of nodes that exist. this enum makes readability better

typedef struct tree_t{
	NodeType type;
	int value;
	struct tree_t* left;
	struct tree_t* right;
	struct tree_t* center; // some operations could be ternary, and therefore the only reasonable workaround if I wanted to avoid ternary trees would have been having the condition be right child, and the two outcomes be in the right subtree. This would have meant I'd have needed to add an argument to my tree-parser function to make it remember the previous result, which I did not feel like doing. It is so much cleaner to have all steps on one layer as opposed to two.
} tree_t;

// math
int myLog10(int n);
int myPow(int a, int b);

// execution-tree
tree_t* createTree(NodeType type, int value);
void printTree(tree_t* node);
void freeTree(tree_t* node);
int parseTree(tree_t* node);

// syntax-recognition
char isOp_1(char c);
char isOp_2(char c);
char isOp_3(char c);
char isComment(char c);
char isLoop(char c);
char isVar(char c);

// parser
void preParse(char** expression);
tree_t* parseE(char** expression);

void freeAll(void);
void printVars(void);

// having the following be global makes preventing memory-leaks and access SIGNIFICANTLY less of a headache
tree_t* root; // instruction tree root which we'll want to access everywhere in the program
tree_t* vars[ALPHABET_SIZE]; // hey uh so these are functions now...? I'm not sure how to handle variable scope yet.

int main(int argc, char** argv){
	char expression[BUFFER_SIZE];
	if(argc < 2){
		printf("No filename entered in cmd line, evaluating hardcoded string instead:\n");
		strcpy(expression, PROG);
	}
	else{
		printf("entered an additional cmd line arg, evaluating file:\n");
		FILE* file = fopen(argv[1], "r");
		if(!fread(expression, 1, BUFFER_SIZE, file)) return FileOpenError;
	}
	char* progCounter = expression; // this is a convenience-related idea I had when starting out this implementation because I was back on the "read the whole expression from a string" pain-train. this way, I don't lose track of the start of the expression. didn't end up needing it but I very well could have.

	// parse into bin-tree
	root = parseE(&progCounter);

	printTree(root);
	printf("\n");

	int result = parseTree(root);
	printf("-> %d\n", result);

	freeTree(root);
	
	printVars();

	return NoError;
}

void preParse(char** expression){

	while(!IS_EOF){

		SKIP_UNNECESSARY;
		
		if(**expression != '='){
			(*expression)++;
		}
		SKIP_UNNECESSARY;
		
		if(**expression >= 'A' && **expression <= 'Z'){
			if(vars[**expression - 'A']){ // garbage-collection any time variables are overwritten
				freeTree(vars[**expression - 'A']);
			}
			vars[**expression - 'A'] = parseE(expression + 1);
			(*expression)++;
		}
		else{ // only capital letters are accepted as possible vars
			printf("invalid definition to %c\n", **expression);
			freeAll();
			exit(ParserError);
		}
	}
}

// parse expression to build the bin tree
tree_t* parseE(char** expression){
	if(IS_EOF) return NULL;

	SKIP_UNNECESSARY;

	if(**expression == '='){
		(*expression)++;
		SKIP_UNNECESSARY;
		if(**expression >= 'A' && **expression <= 'Z'){
			while(**expression != 'e' && !IS_EOF){
				SKIP_UNNECESSARY;
			}
		}
	}

	SKIP_UNNECESSARY;

	if(isOp_1(**expression)){
		char operator = **expression;
		(*expression)++;
		tree_t* node = createTree(OPERATOR, operator);
		node->left = parseE(expression);
		return node;
	}
	
	if(isOp_2(**expression)){
		char operator = **expression;
		(*expression)++;
		tree_t* node = createTree(OPERATOR, operator);
		node->left = parseE(expression);
		node->right = parseE(expression);
		return node;
	}

	if(isOp_3(**expression)){
		char operator = **expression;
		(*expression)++;
		tree_t* node = createTree(OPERATOR, operator);
		node->left = parseE(expression);
		node->right = parseE(expression);
		node->center = parseE(expression);
		return node;
	}

	if(isVar(**expression)){
		tree_t* node = createTree(VARIABLE, **expression);;
		(*expression)++;
		return node;
	}

	// numbers lol lmao
	if(isdigit(**expression)){
		char* start = *expression;
		while(isdigit(**expression)) (*expression)++;  // Advance through all digits
		return createTree(OPERAND, atoi(start));
	}

	return NULL;
}

int myLog10(int n){
	int i;
	for(i = 1; n/10; i++){
		n = n/10;
	}
	return i;
}

int myPow(int a, int b){
	int n = 1;
	while(b > 0){
		n = n * a;
		b--;
	}
	return n;
}

// operation-types
char isOp_1(char c){
	return c == '!'
		|| c == '#';
}

char isOp_2(char c){
	return(c == '+'
		|| c == '-'
		|| c == '*'
		|| c == '/'
		|| c == '^'
	);
}

char isOp_3(char c){
	return c == '?';
}

// loops
char isLoop(char c){
	return c == 'l';
}

// (global) variables
char isVar(char c){
	return c >= 'A'
	    && c <= 'Z'; // Yes, there are only 26 possible variable names. Use them wisely.
}

// (block-)comments
char isComment(char c){
	return c == '\'';
}

// create new tree_t node
tree_t* createTree(NodeType type, int value){
	tree_t* new = (tree_t*) malloc(sizeof(tree_t));
	new->type = type;
	new->value = value;
	new->left = new->right = new->center = NULL;
	return new;
}

// I hate memory leaks
void freeTree(tree_t* node){
	if(node){
		freeTree(node->left);
		freeTree(node->right);
		free(node);
	}
}

// print tree. should return the same thing as was inputted, with additional [] for readability.
void printTree(tree_t* node){
	if(!node){
		printf("(0)"); // (0) here means "assumed to be 0 for lack of better information"
		return;
	}

	if(node->type == OPERATOR){
		printf("[");

		if(isOp_1(node->value)){
			printf("%c", node->value);
			printTree(node->left);
		} 

		if(isOp_2(node->value)){
			printTree(node->left);
			printf("%c", node->value);
			printTree(node->right);
		}

		if(isOp_3(node->value)){
			printTree(node->left);
			printf("%c", node->value);
			printTree(node->right);
			printTree(node->center);
		}

		printf("]");
	}

	else if(node->type == OPERAND){
		printf("%d", node->value);
	}

	else if(node->type == VARIABLE){
		printf("%c", node->value);
	}

	else if(node->type == LOOP){
		printf("[while ");
		printTree(node->left);
		printf(" do ");
		printTree(node->right);
		printf("]");
	}
}

int parseTree(tree_t* node){
	if(!node) return 0;

	if(node->type == OPERATOR){
		int a = parseTree(node->left);
		int b = parseTree(node->right);
		int c = parseTree(node->center);
		
		switch(node->value)
		{
			case '#':
				return parseTree(node->left);

			case '+':
				return a + b;

			case '-':
				return a - b;
			
			case '*':
				return a * b;

			case '/':
				if(b != 0) return a / b;
				freeAll();
				printf("Error: Division by 0.\n");
				exit(ExecError);
			
			case '^':
				return myPow(a, b);

			case '!':
				return !a;

			case '&':
				return a && b;

			case '|':
				return a || b;

			case 'l':
				return a ? parseTree(node) : b;
			
			case '?':
				return a ? b : c;
			
			case '<':
				return a < b;

			case '>':
				return a > b;

			default:
				printf("Error: Unknown operator found: %c", node->value);
				freeAll();
				exit(ExecError);
		}
	}

	if(node->type == OPERAND){
		return node->value;
	}

	return 0;
}

void freeAll(void){
	freeTree(root);
	for(int i = 0; i < ALPHABET_SIZE; i++){
		if(vars[i]) freeTree(vars[i]);
	}
}

void printVars(void){
	printf("[\n");
	for(int i = 0; i < ALPHABET_SIZE; i++){
		printf("%d = %d\n", i, parseTree(vars[i]));
	}
	printf("]\n");
}
