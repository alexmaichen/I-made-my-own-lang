/*
loosely inspired by the following Computerphile video: https://www.youtube.com/watch?v=Q2UDHY5as90
*/

// edit FILENAME constant to edit path to file to parse
#define PROG "/ * + 1 - 4 2 3 3" // -> 3

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

enum{
	NoError,
	ParserError, // thought it would be necessary, but became unused
	ExecError
}; // errorcodes

typedef enum{
	OPERATOR,
	OPERAND,
	FOR_LOOP
} NodeType; // types of nodes that exist. this enum makes readability better

typedef struct tree_t{
	NodeType type;
	int value;
	struct tree_t* left;
	struct tree_t* right;
} tree_t;

tree_t* createTree(NodeType type, int value);
void printTree(tree_t* node);
void freeTree(tree_t* node);
char isOperator(char c);
char isFor(char* expr);
tree_t* parseE(char** expression);
int parseTree(tree_t* node);

tree_t* root; // having this be global makes preventing memory-leaks SIGNIFICANTLY less of a headache

int main(int argc, char** argv){
	char* expression;
	if(argc < 2){
		printf("No additional cmd line args entered, evaluating hardcoded program instead:\n");
		expression = PROG;
	}
	else{
		printf("entered an additional cmd line arg, evaluating argument:\n");
		expression = argv[1];
	}
	char* progCounter = expression;

	// parse into bin-tree
	root = parseE(&progCounter);

	printTree(root);
	printf("\n");

	int result = parseTree(root);
	printf("%d", result);

	freeTree(root);

	return NoError;
}

// create new tree_t type node
tree_t* createTree(NodeType type, int value){
	tree_t* new =(tree_t*) malloc(sizeof(tree_t));
	new->type = type;
	new->value = value;
	new->left = new->right = NULL;
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

// print tree (prefixed). should return the same thing as was inputted, with additional [] for readability
void printTree(tree_t* node){
	if(!node){
		return;
	}

	if(node->type == OPERATOR){
		if(isOperator(node->value)){
			printf("[");
		}
		printTree(node->left);
		printf("%c", node->value);
		printTree(node->right);
		if (isOperator(node->value)){
			printf("]");
		}
	}
	else if(node->type == OPERAND){
		printf("%d", node->value);
	}
	else if(node->type == FOR_LOOP){ //unimplemented
		printf("for ");
		printTree(node->left);
		printf(" do ");
		printTree(node->right);
	}
}

int parseTree(tree_t* node){
	if(node->type == OPERATOR){
		switch(node->value)
		{
			case '+':
				return parseTree(node->left) + parseTree(node->right);

			case '-':
				return parseTree(node->left) - parseTree(node->right);
			
			case '*':
				return parseTree(node->left) * parseTree(node->right);

			case '/':
				return parseTree(node->left) / parseTree(node->right);

			default:
				printf("unknown operator found: %c", node->value);
				freeTree(root);
				exit(ExecError);
		}
	}
	if(node->type == OPERAND){
		return node->value;
	}
	if(node->type == FOR_LOOP){
		return 0; // unimplemented
	}
	return 0;
}

// op-check
char isOperator(char c){
	return(c == '+'
		|| c == '-'
		|| c == '*'
		|| c == '/'
	);
}

// unimplemented, don't use this in your code. forloop-check
char isFor(char* expr){
	return strncmp(expr, "f", 1) == 0; // originally wanted to write "for" to start loops but after a while it became a nightmare to handle so I scrapped it
}

// parse expression to build the bin tree
tree_t* parseE(char** expression){

	if(**expression == '\0') return NULL;
	if(**expression == EOF) return NULL;

	// whitespace is irrelevant
	while(**expression == ' ') (*expression)++;

	// unimplemented, please don't use this in your code. forloop
	if(isFor(*expression)){
		(*expression)++; // skip over f and the operation to perform
		while(**expression == ' ') (*expression)++; // skip spaces

		tree_t* condition = parseE(expression);
		tree_t* body = parseE(expression);
		tree_t* node = createTree(FOR_LOOP, 'f');
		node->left = condition;
		node->right = body;
		return node;
	}

	// + - * / ^
	if(isOperator(**expression)){
		char operator = **expression;
		(*expression)++;
		tree_t* node = createTree(OPERATOR, operator);
		node->left = parseE(expression);
		node->right = parseE(expression);
		return node;
	}

	// numbers lol lmao
	if(isdigit(**expression)){
		char* operand = *expression;
		(*expression)++;
		return createTree(OPERAND, atoi(operand));
	}

	return NULL;
}

/*
// a more recent implementation idea: have every node in the tree have a value, a left arg, a right arg, a left operation, a right operation.
// gave up on this too because it makes writing the code that actually RUNS the code a nightmare.

// info on the following three macros found at https://stackoverflow.com/questions/7972785/can-a-c-macro-definition-refer-to-other-macros
#define aStart 'A'
#define aEnd 'Z'
#define MAX_VARS(aEnd - aStart) //recommend setting this no higher than 29. Attempting to use operators as a variable names will result in unintended / undefined behavior.
#define MAXV(a,b)(a>b?a:b)
#define MAX_TOKENSIZE 1
enum{
	NoError, //added this to make errors start at 1. no more confusing CmdArgError and a successful exit!
	CmdArgError,
	FileOpenError,
	InvalidOperationError,
	ValueError,
	RaisedError,
	SyntaxError
}; // errorcodes

typedef number int;
typedef operation char;
typedef struct tree{
	operation op;
	tree* opL; //only filled if tree.op != '\0'
	tree* opR; //only filled if tree.op != '\0'
	number left; //only filled if tree.op == '\0'
	number right; //only filled if tree.op == '\0'
} tree_t;

tree_t treeRoot; // having globals is unclean, but this spares me some reference-passing in functions

int vars[MAX_VARS]; // unimplemented

int main(void){
	FILE *file = fopen(FILENAME, "r");
	if(!file){
		printf("Could not open file '%s'\n", FILENAME);
		return 1;
	}

	// unimplemented
	for(int i = 0; i < MAX_VARS; i++){
		vars[i] = 0;
	}

	fileR(file);
	//printVars();

	freeTree();
	fclose(file);
	return 0;
}

// parse file
int fileR(FILE* file){
	int nbRead = 0;
	char token[MAX_TOKEN_SIZE]; // originally thought I'd use more than one character per operation or operand, as it turns out I didn't
	token[0] = '\0';
	while(token != EOF){
		while(token != ' '
		&& token != '\n'){
			if(token[0] == EOF){freeTree(treeRoot); return nbRead;}
			nbRead++;
			if(!fread(token, 1, char, file)) break;
		}
		if(token[0] == '!'){ // essentially a "raise" operator
			printf("Error: A custom error was raised / a breakpoint was crossed.");
			//printVars();
			freeTree(treeRoot);
			exit(RaisedError);
		}

		if(isdigit(token[0])){ // number encountered
			addChildAt(findNextFreeSpace(treeRoot,'n'), atoi(token)); // single-digit numbersz only!
		}
		
		else{
			addChildAt(findNextFreeSpace(treeRoot, 'o'), token[0]);
		}
	}
}

tree_t* addTreeNode(operation op, number left, number right){
	tree_t* new = malloc(sizeof tree_t);
	new->op = op;
	new->left = left;
	new->right = right;
	new->opL = 0;
	new->opR = 0;
	return new;
}

// create a new node in the tree
void addChildAt(tree_t* node, operation op, number n){
	if(!op){
		if(!node->left){
			node->left = n;
			return;
		}
		if(node->right){
			node->right = n;
			return;
		}
		return;
	}
	if(!node->op){

		if(node->opR && node->opR){
			return;
		}
	}
	return;
}

// safely free the entire bintree structure
void freeTree(tree_t node){
	if(!node){return;}
	freeTree(node->opL);
	freeTree(node->opR);
	free(node);
}

// recurse until you hit the first possible free spot
tree_t* findNextFreeSpace(tree_t* node, char lookingFor){ // lookingFor n -> number, o -> operation
	if(!node) return 0;
	switch(lookingFor){
		case 'o': if(node->op == '\0') return node->op;
		case 'n':
			if(node->left == -1) return node->left;
			if(node->right == -1) return node->right;
		default: return 0; // could not match request
	}
	return MAXV(findNextFreeSpace(node->opL), findNextFreeSpace(node->opR)); // arbitrary choice of using the pointer towards the larger of the two. this is to avoid picking always on the same side.
}

// perform calculations with one of + - * / ^
int performOperation(tree_t* node){
	switch(node->op){
		case '+': return node->left + node->right;
		case '-': return node->left - node->right;
		case '*': return node->left * node->right;
		case '/':
			if(node->right != 0){
				return node->left / node->right;
			}
			else{
				printf("Error: Division by zero.\n");
				freeTree(treeRoot);
				exit(ValueError);
			}
		case '^': return(int) pow((double) node->left,(double) node->right);
		default: 
			printf("Error: Unknown operator: %c\n", node->op);
			freeTree(treeRoot);
			exit(InvalidOperationError);
	}
}

// unimplemented
int getVar(char var){
	if(var >= aStart && var <= aEnd){
		return vars[var - aStart];
	}
	printf("Error: Undefined variable '%c'\n", var);
	freeTree(treeRoot);
	exit(InvalidOperationError);
}

// unimplemented
void setVar(char var, int value){
	if(var >= aStart && var <= aEnd){
		vars[var - aStart] = value;
		return;
	}
	printf("Error: Invalid variable '%c'\n", var);
	freeTree(treeRoot);
	exit(InvalidOperationError);
}

// unimplemented
void printVars(void){
	// print out all variables and their values
	for(char v = aStart; v < aEnd; v++) printf("%c = %d\n", v, vars[v]);
}
*/

/*
// previous implementation idea: incomplete + mildly botched attempt which used manual memory-allocation. I recommend not looking at this, for your own sake.
// I later noticed that having a prefixed interpreter would be an utter nightmare using this implementation so I turned away from this.

int main(int argc, char* argv[]){
	if(argc < 2){
		exit(CmdArgNbError);
	}

FILE *file = fopen(FILENAME, "r");
if(!file){
printf("Could not open file '%s'\n", FILENAME);
exit(FileOpenError);
}
int stack[MAX_STACK_SIZE];
	int top = -1; // keep track of the element to use
int vars[MAXVARS];
	calc(file);

// when all is done, the result is the last element in stack
if(!top){
printf("Result: %d\n", pop());
}
	else{
printf("Error: The stack is not empty, something went wrong.\n");
fclose(file);
exit(StackNotEmptyError);
}
return 0;
}

// stack-related
void push(int* top, int value){
if(top < MAX_STACK_SIZE - 1){
		*top += 1;
stack[top] = value;
}
	else{
printf("Stack Overflow!\n");
}
}

int pop(int* top){
if(top < 0){
printf("Error: Stack Underflow\n");
exit(StackUnderflowError);
}
	*top -= 1;
return stack[top];
}

// read file with code
void rFile(int* top, FILE *file){
char line[BUFFER_SIZE];
char token[BUFFER_SIZE];

	// initialize everything to 0
	for(int i = 0; i < MAXVARS; i++){
varray[i] = 0;
}

// read file line by line
while(fscanf(file, "%s", token) != EOF){
if(!strcmp(token, "while")){
handle_while(file);
}
		
		else if(isalpha(token[0]) && strlen(token) == 1){
char var = token[0];
fscanf(file, "%s", token); // looking for '='
fscanf(file, "%s", token); // value to assign
int value = atoi(token);
set_variable_value(var, value);
}
		
		else if(token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/' || token[0] == '^'){
// Process an operation
int operand1 = evaluate_expression(file);
int operand2 = evaluate_expression(file);
int result = perform_operation(token[0], operand1, operand2);
push(result);
}
}

// Print the final result if there is one
if(top == 0){
printf("Result: %d\n", pop(top));
}
	
	else{
printf("Error: The stack is not empty, something went wrong.\n");
}

fclose(file);
}

// operations
int calc(char op, int a, int b){
switch(op){
case '+': return a+b;
case '-': return a-b;
case '*': return a*b;
case '/':
if(b){ // legal
return a/b; // integer division
}
			else{ // division by 0 is impossible
printf("Error: attempted to divide by 0\n");
exit(ValueError);
}
		case '^': return a^b;
default: // invalid operation
printf("Error: invalid operation: %c\n", op);
			exit(InvalidOperationError);
}
}

int getVar(char var){
	if(var >= aStart && var <= aEnd){
return varray[var - aStart];  // Convert char to index
}
printf("Error: invalid variable '%c'\n", var);
exit(InvalidOperationError);  // Exit on undefined variable
}

int setVar(char var, int val){
	if(var >= aStart && var <= aEnd){
varray[var - aStart] = val;
}
	
	else{
printf("Error: invalid variable '%c'\n", var);
exit(InvalidOperationError);  // Exit on invalid variable
}
}

void whileLoops(FILE *file){
	char token[BUFFER_SIZE];
fscanf(file, "%s", token);

if(strcmp(token, "while")){
printf("Error: Expected 'while', got '%s'\n", token);
exit(InvalidOperationError);
}

// eval condition
int condition = calc(file);

while(condition){
// eval contents
fscanf(file, "%s", token);

// Assuming we read "begin" for starting loop body
if(strcmp(token, "begin")){
printf("Error: Expected 'begin', got '%s'\n", token);
exit(InvalidOperationError);
}

// Process the body of the loop(let's assume it's a simple expression or sequence of expressions)
calc(file); // This is just a simple example; you can expand it to handle multiple statements

// Evaluate the condition again after running the loop body
condition = calc(file);
}
}
*/
