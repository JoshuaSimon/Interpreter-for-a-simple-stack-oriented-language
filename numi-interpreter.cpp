// Numi-Level1.cpp
// Interpreter for the Numi language written in C++ for top speed and performance.
// Author: Joshua Simon
// Date: 17.08.2019
// Verison: 0.3

#include <iostream>
#include <deque>
#include <string>
#include <map>
using namespace std;

// Nodetypes
enum nodetype {
	FunctionCall,
	NumberLiteral,
	Error
};

// Node data type
struct Node {
	nodetype type;
	string value;
};

// Function pointer type
//typedef deque<Node> (*pfunc)(deque<Node>)
typedef double (*pfunc)(double, double);

// Pointer type for deque<Node> 
typedef deque<Node>(*pdeque);

// Parse the source code to the stack
deque<Node> parse2(string source);

// Turns a single number into Node
Node parse_number(string c);

// Turns single function into Node
Node parse_function(string c);

// Truns function of type double (double, double) to Noda/stack<Node>
//stack<Node> num_binop(double f(double, double), stack<Node> stack);

// Executs function and pushes on the stack
void execute_fun_on_stack(Node instruction, pdeque instruction_stack, pdeque e_stack, map<string, pfunc> FunctionMap);

// Prints all elements of deque of type deque<Node>
void print_deque(deque<Node> myNodeDeque);


// Basic calulation functions
double add(double a, double b) { return a + b; };
double sub(double a, double b) { return a - b; };
double mul(double a, double b) { return a * b; };
double div(double a, double b) { return a / b; };
double rosenbrock(double x, double y) { return pow(1.0 - x, 2.0) + 100.0 * pow(y - x * x, 2.0); }


class Interpreter {
public:
	deque<Node> instructions;				// Instruction stack
	map<string, pfunc> FunctionMap;			// Function map

	string source;							// Stirng that holds one line of source code
	Interpreter();						    // Constructer
	deque<Node> interprete(string source);	// Interprets one line of source code and save the result stack
	void printResults(deque<Node> e_stack); // Prints the result stack to command line window

	void addFunctions();					// Add functions to Function Map
};


int main()
{
	// Print some information. 
	cout << "-------------------------------------------------------------------" << endl;
	cout << "Numi Interpreter Version 0.3. Enter \"exit\" to close the programm." << endl;
	cout << "-------------------------------------------------------------------" << endl;
	cout << endl << endl;


	// Read source from command line until exit command is entered.
	string source;
	Interpreter I;

	while (true) {
		cout << "Numi > ";
		getline(cin, source);

		if (source == "exit" || source == "EXIT") {
			return 0;
		}

		deque<Node> result = I.interprete(source);
		I.printResults(result);
	}
	
	return 0;
}

deque<Node> parse2(string source)
{
	deque<Node> instructions;

	while (!source.empty()) {
		string buf;
		char c = source.front();
		source.erase(source.begin());

		if (isdigit(c)) {

			buf.push_back(c);

			while (!source.empty()) {
				char cs = source.front();
				source.erase(source.begin());

				if (isdigit(cs) || cs == '.') {
					buf.push_back(cs);
				}
				else {
					source.insert(0, 1, cs);
					break;
				}
			}
			instructions.push_back(parse_number(buf));
			//cout << "Is digit: " << buf << endl;
		}
		else if (isspace(c)) {
			//cout << "Is space: " << c << endl;
		}
		else {

			buf.push_back(c);

			while (!source.empty()) {
				char cs = source.front();
				source.erase(source.begin());

				if (!isspace(cs)) {
					buf.push_back(cs);
				}
				else {
					source.insert(0, 1, cs);
					break;
				}
			}
			instructions.push_back(parse_function(buf));
			//cout << "Is function: " << buf << endl;
		}

	}

	return instructions;
}

Node parse_number(string c)
{
	Node newNumber;
	newNumber.type = NumberLiteral;
	newNumber.value = c;

	return newNumber;
}

Node parse_function(string c)
{
	Node newFunction;
	newFunction.type = FunctionCall;
	newFunction.value = c;

	return newFunction;
}

/*
stack<Node> num_binop(double f(double, double), stack<Node> stack)
{
	Node Result;

	return;
}
*/

void execute_fun_on_stack(Node instruction, pdeque instruction_stack, pdeque e_stack, map<string, pfunc> FunctionMap)
{
	double a, b, c;
	Node result;
	Node functionCall = instruction;
	instruction_stack->pop_front();

	// Checks if function is in function map
	if (FunctionMap.count(functionCall.value) == 0) {
		cout << "Error: " << functionCall.value << " is no build in Function" << endl;
		c = 0.0;
		result.type = NumberLiteral;
		result.value = to_string(c);

		e_stack->push_back(result);

		return;
	}
	else {
		Node l = e_stack->back();
		e_stack->pop_back();
		Node r = e_stack->back();
		e_stack->pop_back();

		// Convert the value of a Node into a double 
		a = stod(l.value);
		b = stod(r.value);

		pfunc add = FunctionMap["+"];
		pfunc sub = FunctionMap["-"];
		pfunc mul = FunctionMap["*"];
		pfunc div = FunctionMap["/"];
		pfunc rosenbrock = FunctionMap["rosenbrock"];

		if (functionCall.value == "+") {
			c = (*add)(a, b);
		}
		else if (functionCall.value == "-") {
			c = (*sub)(a, b);
		}
		else if (functionCall.value == "*") {
			c = (*mul)(a, b);
		}
		else if (functionCall.value == "/") {
			c = (*div)(a, b);
		}
		else if (functionCall.value == "rosenbrock") {
			c = (*rosenbrock)(a, b);
		}

		result.type = NumberLiteral;
		result.value = to_string(c);

		e_stack->push_back(result);

		return;
	}
}


void print_deque(deque<Node> myNodeDeque)
{
	while (!myNodeDeque.empty()) {
		Node printNode = myNodeDeque.back();
		cout << ' ' << printNode.value;
		myNodeDeque.pop_back();
	}
	return;
}

Interpreter::Interpreter()
{
	addFunctions();
	return;
}

deque<Node> Interpreter::interprete(string source)
{
	deque<Node> e_stack;
	deque<Node> instruction_stack = parse2(source);

	while (!instruction_stack.empty()) {
		Node instruction = instruction_stack.front();

		if (instruction.type == NumberLiteral) {
			e_stack.push_back(instruction);
			instruction_stack.pop_front();
		}
		else if (instruction.type == FunctionCall) {
			execute_fun_on_stack(instruction, &instruction_stack, &e_stack, FunctionMap);
		}
		else {
			cout << "ERROR: Can't interprete source." << endl;
			instruction_stack.pop_front();
		}
	}

	return e_stack;
}

void Interpreter::printResults(deque<Node> e_stack)
{
	cout << "Output:   ";
	while (!e_stack.empty()) {
		Node printNode = e_stack.back();
		cout << ' ' << printNode.value;
		e_stack.pop_back();
	}
	cout << endl << endl;

	return;
}

void Interpreter::addFunctions()
{
	FunctionMap["+"] = add;
	FunctionMap["-"] = sub;
	FunctionMap["*"] = mul;
	FunctionMap["/"] = div;
	FunctionMap["rosenbrock"] = rosenbrock;
}

