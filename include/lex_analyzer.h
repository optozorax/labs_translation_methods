#pragma once

enum SymbolType
{
	SYMBOL, // "_a-zA-Z"
	DOT,    // "."
	EXCL,   // "!"
	EQUAL,  // "="
	NUMBER, // "0-9"
	DIV,    // "/"
	MUL,    // "*"
	EOLN,   // "\n"
	DELIM,  // ";,+-<>"
	OTHER,
};

enum StateType
{
	Start, PUSH, ADD, ERR,
	Com1, Com2, Com3, Com4,
	Num1, Num2,
	Del1, Del2,
};

// f(State, Symbol) = new State
int lex_table[][] = {
	// ...
};