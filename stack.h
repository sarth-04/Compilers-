#include <parserDef.h>
#include <stdlib.h>

typedef struct ParseTreeStack {
	int capacity;
	int top; // Index of the top element.
	ParseTreeNode** elements;
} ParseTreeStack;

// Create a new stack with an initial capacity.
ParseTreeStack* createStack(int capacity) {
	ParseTreeStack* stack = (ParseTreeStack*) malloc(sizeof(ParseTreeStack));
	stack->capacity = capacity;
	stack->top = -1;
	stack->elements = (ParseTreeNode**) malloc(capacity * sizeof(ParseTreeNode*));
	return stack;
}

bool isEmpty(ParseTreeStack* stack) {
	return (stack->top == -1);
}

void push(ParseTreeStack* stack, ParseTreeNode* node) {
	// Resize the stack if needed.
	if (stack->top + 1 >= stack->capacity) {
		stack->capacity *= 2;
		stack->elements = (ParseTreeNode**) (stack->elements, stack->capacity * sizeof(ParseTreeNode*));
	}
	stack->elements[++stack->top] = node;
}

ParseTreeNode* pop(ParseTreeStack* stack) {
	if (isEmpty(stack))
		return NULL;
	return stack->elements[stack->top--];
}

ParseTreeNode* top(ParseTreeStack* stack) {
	if (isEmpty(stack))
		return NULL;
	return stack->elements[stack->top];
}