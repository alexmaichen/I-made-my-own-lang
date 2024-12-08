# I-made-my-own-lang
There comes a point at which every programmer wonders how programming languages read the user's code. At least I know I have in the past. Well, working on this I am slowly gaining more and more respect for people that have, because as it turns out anything outside of postfixed parsing is a complete nightmare.
You may have noticed the two massive block-comments at the tail-end of the file. These have old implementation-ideas that were then scrapped. Follow the following hyperlinks to see the details to all versions.
- [v3](#v3)
- [v2](#v2)
- [v1](#v1)

Approximate time spent on this project: 130 hours

# v3
## general information
This language is prefixed. That means you start with the instuction, and then give the operands to use it on.
- Thus, the sum of 3 and 8 is written as "+ 3 8"
- Operations can also be nested: "+ 3 * 2 4"
- Due to its prefixed nature, parentheses are irrelevant. There is no PEMDAS operation order here. Same property applies to postfixed langs.
- Once it runs out of instructions to fill with values, the program stops reading. this allows for comments to be added after the code. This means the code "+ 3 * 2 4 Hello World" should return the same thing as "+ 3 * 2 4"
- There are only integers in this language. Behavior of floats in the input-string is undefined.
- Integers with multiple digits are supported.
- The possible operations are + addition, - subtraction, * multiplication, / division, ^ exponentiation
- at most 256 characters will be read from files (because I was too lazy to manually manage memory for my input).

To interpret your code, compile main.c and run the resulting executable.
- Without command line arguments if you wish to directly work with the hard-coded macro strings in main.c
- With command line arguments if you wish to read from a text file

The interpreter uses a binary tree to parse the program. Every instuction only has two operands, so this approach works wonders.

## functions
tree_t* createTree(NodeType type, int value);
- Allocates memory for a tree node and returns a pointer to this node.
- Fills the node with information about this piece of data (the value and what it should be read as)

void printTree(tree_t* node);
- Prints the tree in infixed notation so the user can quickly spot where potential errors stem from
- Parses the tree recusively

void freeTree(tree_t* node);
- Frees all memory allocated to the tree stucture.

char isOperator(char c);
- Is used to check if the node's value attribute is that of a valid operation.

tree_t* parseE(char** expression);
- Parses expression read from PROG macro or from file given in cmd-line args.

int parseTree(tree_t* node);
- Parses tree created from read string. Starts at the tree's root and works recursively downwards.

int myLog10(int n);
- Some compilers do not know the log10(double) macro in the math.h header. That is why I created my own, which works well enough for the purposes of this project.
- Returns the natural logarithm (base 10) of the given input.
- Used to determine how far to make the filepointer jump ahead.

int myPow(int a, int b);
- Some compilers do not know the pow(double, double) macro in the math.h header. That is why I created my own, which works well enough for the purposes of this project.
- Returns to compute a to the power of b.

# v2
## main differences
- This had all parts of the operation (so both operands and the operator) inside the tree-node. After calculating a result in one of the two tree-branches, the result for that tree-banch would be saved in the layer above.
- Unfortunately, this quickly became cumbersome to work with so I scrapped it.
- For this version I had planned more error-handling (more possible errorcodes, and a ! operator to raise an error and print everything out, which would essentially act as a debugging tool) and still used a static token-size of 1. I originally planned to allow for longer tokens, but after some thinking I realized I could simply use 1 character for every basic operation.
- I defined custom types for operations and numbers, which while making my code more readable made it too abstract for me to work with effectively. For that reason, those types were scrapped for v3
- This implementation did not yet have the proper parsing of numbers like v3 does because I did not know about the int atoi(char*) function yet.

# v1
## main differences
- This started with the stack-approach seen for postfixed interpreters as seen in [this Computerphile YouTube video](https://www.youtube.com/watch?v=Q2UDHY5as90) about the basics of writing an interpreter. The stack had a top-pointer to say where the most recently added element was put.
- I tried this idea as a concept (and never tested the code in v1, I just wanted to see what it might look like) but quickly realized I'd have to rewrite a bunch of the code anyway to apply this to a prefixed syntax so I scrapped the idea.
