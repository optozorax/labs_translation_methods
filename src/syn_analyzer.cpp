#include <string>
#include <syn_analyzer.h>
#include <fstream>
#include <iostream>

class SynTable
{
public:
	SynTable(const std::string& path, Tables& tables);		//Конструктор

	void Next(Token);					//Обработака следующего токена

private:

	struct  Row							//Строка лексической таблицы
	{
		std::vector<string> term;
		int jmp;
		bool act;
		bool stk;
		bool ret;
		bool err;
	};


	Type type;							//Тип следующих функций
	Tables& tables;						//Хэш таблицы
	int state;							//Текущее состояние
	std::vector<int> stack_state;		//Стек следующих состояний
	std::vector<Row> syn_table;			//Синтаксическая таблица
};

//=============================================================================
//=============================================================================
//=============================================================================

SynTable::SynTable(const std::string& path, Tables& tables) : tables(tables) {
	ifstream itable(path);

	if (!itable)
	{
		string error = "Error open file " + path;
		throw error;
	}

	int n, m;
	int num;

	itable >> n;

	syn_table.resize(n);

	stack_state.clear(); 
	state = 0;
	type = TYPE_NONE;

	for (size_t i = 0; i < n; i++)
	{
		itable >> m;
		syn_table[i].term.resize(m);
		for (size_t j = 0; j < m; j++)
			itable >> syn_table[i].term[j];

		itable >> syn_table[i].jmp;
		itable >> syn_table[i].act;
		itable >> syn_table[i].stk;
		itable >> syn_table[i].ret;
		itable >> syn_table[i].err;
	}

	itable.close();
}

std::optional<> SynTable::Next(Token token)
{
	bool f = false;
	for (auto& i : syn_table[state].term)
	{
		if (i == "indS" && token.table == TABLE_IDENTIFIERS)
		{
			auto b = tables.getStr(token);		//доработать
			f = (b[0] >= 'A' && b[0] <= 'Z');
		}
		if (i == "ind" && token.table == TABLE_IDENTIFIERS)
			f = true;
		if (i == "const" && token.table == TABLE_CONSTANTS)
			f = true;
		if (token == tables.find(i))
			f = true;
	}

	int s;

	if (f)
	{
		if (syn_table[state].stk)
			stack_state.push_back(state + 1);

		s = state;

		a = b = c
		b = (a+c)*(b-d)
		b = a + c * d
		b = a * c + d

		//Тип задаем 
		switch (state)
		{
		case 28:
			type = TYPE_INT;
			break;
		case 29:
			type = TYPE_FLOAT;
			break;
		case 30:
			type = TYPE_STRUCT;
			break;
		case 23:
		case 43:
			type = TYPE_NONE;
		case 21:
		case 49:
			if (type != TYPE_NONE)
			{
				auto& arg = tables.get<Identifier>(token);
				arg.type = type;
			}
			break;
		default:
			break;
		}

		if (syn_table[state].ret)
		{
			if (stack_state.size()) {
				state = stack_state.back();
				stack_state.pop_back();
			} 
			//else
				//return;
			
		}
		else
			state = syn_table[state].jmp;

		if (!syn_table[s].act)
			Next(token);
	}
	else
		if (!syn_table[state].err)
		{
			state++;
			Next(token);
		}
		else
		{
			std::string error = "Error, instead "+ tables.getStr(token) + " expected: ";
			for (auto& i : syn_table[state].term)
				error += i;
			throw error;
		}
}

//-----------------------------------------------------------------------------
std::vector<std::vector<string>> Analyzer(const std::vector<Token>& tokens, Tables& tables, string path){
	std::vector<std::vector<string>> result;
	
	SynTable syn_table(path, tables);

	Token space = tables.find(" ");
	Token enter = tables.find("\n");
	Token tab = tables.find("\t");

	for (auto i : tokens)
		if (i.operator!=(space) && i != enter && i != tab)
		{
			//std::cout <<  tables.getStr(i) << std::endl;
			if (syn_table.Next(i)) {
				// push
			}
		}  


	return result;
}