#include <string>
#include <lex_analyzer.h>

enum AddType
{
	ADD_IDENTIFIER,
	ADD_CONSTANT,
	ADD_KEYWORD_OR_DELIMITER,
	IGNORE,

	ERROR,
};

enum StateType
{
	Start,  // Стартовое состояние

	// Завершающие состояния, при достижении их, конечный автомат должен завершать свою работу
	ADD,   // Все считанные символы, начиная со Start, до текущего момента ялвяются лексемой
	PUSH,  // ADD + Поместить предыдущий считанный символ обратно в ввод
	NEXT,  // Данная лексема бесполезна для трансляции, и её можно игнорировать
	ERR,   // Ошибочное состояние
	_IDE,  // PUSH, только для таблицы идентификаторов
	_CONS, // PUSH, только для таблицы констант

	// Промежуточные состояния
	Ide,
	Com1, Com2, Com3, Com4,
	Num1, Num2,
	Oper1, Oper2,
};

enum SymbolType
{
	SYMBOL, // "_a-zA-Z"
	NUMBER, // "0-9"
	DELIM,  // " \t;,{}()"
	OPER,   // "+-<>"

	DOT,    // "."
	EXCL,   // "!"
	EQUAL,  // "="
	DIV,    // "/"
	MUL,    // "*"
	EOLN,   // "\n"

	OTHER,
};

SymbolType getType(char symbol);

// Здесь надо запрогать конечный автомат, который по текущему состоянию и считанному символу возвращает следующее состояние
StateType automatonNext(StateType state, SymbolType symbol);

class Automaton
{
public:
	Automaton(const std::string& str);
	bool next(void);
	bool isEndState(void) const;
	AddType getState(void) const;
	std::string getStr(void) const;
	int getPos(void) const;
	int getLine(void) const;
private:
	const std::string& str;
	int i, n;
	std::string sum;
	StateType state;
};

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
SymbolType getType(char c) {
	if (c == '.') return DOT;
	if (c == '!') return EXCL;
	if (c == '=') return EQUAL;
	if (c == '/') return DIV;
	if (c == '*') return MUL;
	if (c == '\n') return EOLN;

	if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return SYMBOL;
	if (c >= '0' && c <= '9')
		return NUMBER;
	if (c == ' ' || c == '\t' || c == ';' || c == ',' || c == '{' || c == '}' || c == '(' || c == ')') 
		return DELIM;
	if (c == '+' || c == '-' || c == '<' || c == '>')
		return OPER;

	return OTHER;
}

//-----------------------------------------------------------------------------
StateType automatonNext(StateType state, SymbolType sym) {
	switch (state) {
		case Start: {
			if (sym == EQUAL) return Oper1;
			if (sym == EXCL) return Oper2;
			if (sym == SYMBOL) return Ide;
			if (sym == DELIM) return ADD;
			if (sym == OPER) return ADD;
			if (sym == NUMBER) return Num1;
			if (sym == DIV) return Com1;

			if (sym == DOT) return ERR;
			if (sym == MUL) return ADD;
			if (sym == EOLN) return ADD;

			return ERR;
		} break;

		case Ide: {
			if (sym == SYMBOL) return Ide;
			if (sym == NUMBER) return Ide;
			if (sym == DOT) return Ide;
			return _IDE;
		} break;
		case Com1: {
			if (sym == DIV) return Com2;
			if (sym == MUL) return Com3;
			return PUSH;
		}  break;
		case Com2: {
			if (sym == EOLN) return NEXT;
			return Com2;
		}  break;
		case Com3: {
			if (sym == MUL) return Com4;
			return Com3;
		}  break;
		case Com4: {
			if (sym == DIV) return NEXT;
			return Com3;
		} break;
		case Num1: {
			if (sym == NUMBER) return Num1;
			if (sym == DOT) return Num2;
			return _CONS;
		}  break;
		case Num2: {
			if (sym == NUMBER) return Num2;
			return _CONS;
		} break;
		case Oper1: {
			if (sym == EQUAL) return ADD;
			return PUSH;
		}  break;
		case Oper2: {
			if (sym == EQUAL) return ADD;
			return ERR;
		} break;

		case ADD:
		case PUSH:
		case ERR:
		case _IDE:
		case _CONS: {
			throw std::exception("That is final state!");
		} break;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Automaton::Automaton(const std::string& str) : str(str), i(0), n(1), state(Start) {
}

//-----------------------------------------------------------------------------
bool Automaton::next(void) {
	if (state == PUSH || state == _IDE || state == _CONS)
		if (str[--i] == '\n') n--;
	if (isEndState()) {
		state = Start;
		sum = "";
	}

	char c = str[i];

	//Надо либо automatonNext засовывать внуторь класса Automation, либо так
	if (c == '\n')  n++;

	state = automatonNext(state, getType(c));
	sum += c;

	i++;

	return i != str.size();
}

//-----------------------------------------------------------------------------
bool Automaton::isEndState(void) const {
	return 
		state == ADD  ||
		state == PUSH ||
		state == NEXT ||
		state == ERR  ||
		state == _IDE ||
		state == _CONS;
}

//-----------------------------------------------------------------------------
AddType Automaton::getState(void) const {
	if (state == ADD || state == PUSH)
		return ADD_KEYWORD_OR_DELIMITER;
	if (state == _IDE)
		return ADD_IDENTIFIER;
	if (state == _CONS)
		return ADD_CONSTANT;
	if (state == NEXT)
		return IGNORE;
	if (state == ERR)
		return ERROR;

	return ERROR;
}

//-----------------------------------------------------------------------------
std::string Automaton::getStr(void) const {
	auto result = sum;
	if (state == PUSH || state == _IDE || state == _CONS)
		result.pop_back();
	return result;
}

//-----------------------------------------------------------------------------
int Automaton::getPos(void) const {
	return i;
}

int Automaton::getLine(void) const {
	return n;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<Token> parse(const std::string& str, Tables& tables) {
	std::vector<Token> result;
	Automaton automaton(str);
	while (automaton.next()) {
		if (automaton.isEndState()) {
			auto to_add = automaton.getStr();
			switch (automaton.getState()) {
				case ADD_IDENTIFIER: {
					auto token = tables.find(to_add);
					if (token) {
						result.push_back(token);
					} else {
						result.push_back(tables.add(to_add, TABLE_IDENTIFIERS));
					}
				} break;
				case ADD_CONSTANT: {
					auto token = tables.find(to_add);
					if (token) {
						result.push_back(token);
					} else {
						result.push_back(tables.add(to_add, TABLE_CONSTANTS));
					}
				} break;
				case ADD_KEYWORD_OR_DELIMITER: {
					auto token = tables.find(to_add);
					if (token) {
						result.push_back(token);
					} else {
						// Такого быть не может
						throw std::exception();
					}
				} break;
				case IGNORE: {} break;
				case ERROR: {
					// Надо как-то обработать эту ошибку
					std::string error = "Error around symbol \"" + to_add +
						"\"\nIn " + std::to_string(automaton.getLine()) + " line";
					throw error; // std::exception();
				break;}
			}
		}
	}

	return result;
}