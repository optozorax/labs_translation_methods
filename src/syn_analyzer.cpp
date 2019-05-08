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

	string nameStr;						//Имя структуры, которая обрабатывается

	bool CheckTrashToken(Token token);	//Проверка ненужных токенов
	int nstr;							//Номер строки у токенов

	void Error(std::string);

	Token struc;
	Type type;							//Тип следующих функций
	Tables& tables;						//Хэш таблицы
	int state;							//Текущее состояние
	std::vector<int> stack_state;		//Стек следующих состояний
	std::vector<Row> syn_table;			//Синтаксическая таблица
};

//=============================================================================
//=============================================================================
//=============================================================================
//Считываем таблицу и задаем начальные значения
SynTable::SynTable(const std::string& path, Tables& tables) : tables(tables) {
	ifstream itable(path);

	if (!itable) Error("Error open file " + path);

	int n, m;
	int num;

	itable >> n;

	syn_table.resize(n);

	stack_state.clear(); 
	state = 0;
	type = TYPE_NONE;
	nstr = 1;

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

void SynTable::Error(std::string str)
{
	throw "Error(" + std::to_string(nstr) + "): " + str;
}

bool SynTable::CheckTrashToken(Token token)
{
	Token tab = tables.find("\t");
	Token sce = tables.find(" ");
	Token ent = tables.find("\n");
	if (token == tab || token == sce) return true;
	if (token == ent) { nstr++; return true; }
	return false;
}

void/*std::optional<>*/ SynTable::Next(Token token)
{
	if (CheckTrashToken(token)) return;

	bool f = false;
	for (auto& i : syn_table[state].term)
	{
		if (i == "indS" && token.table == TABLE_IDENTIFIERS)
		{
			auto b = tables.getStr(token);		//доработать
			if (b[0] >= 'A' && b[0] <= 'Z') { f = true; break; }
			
		}
		if (i == "ind" && token.table == TABLE_IDENTIFIERS) { f = true; break; }
		if (i == "const" && token.table == TABLE_CONSTANTS) { f = true; break; }
		if (token == tables.find(i)) { f = true; break; }
	}

	int s;

	//Если токен есть в списке возможных слов
	if (f)
	{
		//Добавление в стек
		if (syn_table[state].stk)
			stack_state.push_back(state + 1);

		//Созранение положение
		s = state;

		//a = b = c
		//b = (a+c)*(b-d)
		//b = a + c * d
		//b = a * c + d

		//Задаем тип
		switch (state)
		{
		case 12:	//Добавление тип структуры
			nameStr = tables.getStr(token);
			struc = tables.add(nameStr, TABLE_STRUCTURES);
			break;
		case 21:	//Добавление поля в таблицу структуры
		{
			auto& arg = tables.get<Structure>(struc);
			Structure::StructElem elem;
			string name = tables.getStr(token);

			for (auto& i : arg.elems) //Ошибка одинаковые поля в структуре
				if (i.name == name) Error("ind \"" + name + "\" was announced in struc \"" + nameStr + "\"");

			elem.name = name;
			elem.structToken = struc;
			elem.type = type;
			arg.elems.push_back(elem);
		}
			break;
		case 49: //Объевление переменной
			if (type == TYPE_STRUCT)	//Если тип структура
			{
				auto& arg = tables.get<Structure>(struc);	//То всем полям, которые найдем в таблице
				for (auto& i : arg.elems)					//Задаем тип из структуры
				{
					auto tok = tables.find(tables.getStr(token) + "." + i.name);
					if (tok)
					{
						auto& argtok = tables.get<Identifier>(tok);
						argtok.type = i.type;
					}
				}
			}
			if (type != TYPE_NONE)
			{	//Задаем тип переменной
				auto& arg = tables.get<Identifier>(token);
				if (arg.type != TYPE_NONE) Error("\"" + tables.getStr(token) + "\" ind was announced"); //Ошибка переменная уже была объявлена
				arg.type = type;
			}
			break;
		case 28: //Объявление тип int
			type = TYPE_INT;
			break;
		case 29: //Объявление тип float
			type = TYPE_FLOAT;
			break;
		case 30: //Объявление тип структуры
			type = TYPE_STRUCT;
			struc = tables.find(tables.getStr(token), TABLE_STRUCTURES);
			break;
		case 23:
		case 43: //окончание объявления
		case 53:
			type = TYPE_NONE;
			break;
		default:
			break;
		}

		//Конец ветки, возвращение по стеку
		if (syn_table[state].ret)
		{
			//Если пустой то пишем ошибку(по идее она не случится)
			if (!stack_state.size()) Error("stack going lim"); //Ошибка выход из стека

			state = stack_state.back();
			stack_state.pop_back();
		}
		else
			state = syn_table[state].jmp;

		//Если это не терм то идем с этим токеном дальше
		if (!syn_table[s].act)
			Next(token);
	}
	else
		//Если токен не найден среди возмодный слов
		if (!syn_table[state].err)//и это не ошибка
		{
			state++;		//переходим на альтернативу
			Next(token);
		}
		else		//Иначе формируем сообщение об ошибке
		{
			std::string error = "instead "+ tables.getStr(token) + " expected: ";
			for (auto& i : syn_table[state].term)
				error += i;
			Error(error); //Ошибка встретили не тот токен
		}
}

//-----------------------------------------------------------------------------
std::vector<std::vector<string>> Analyzer(const std::vector<Token>& tokens, Tables& tables, string path){
	std::vector<std::vector<string>> result;
	
	SynTable syn_table(path, tables);

	for (auto i : tokens)
		//std::cout <<  tables.getStr(i) << std::endl;
		//Обрабатываем по одному токену
		syn_table.Next(i);
		//if (syn_table.Next(i)) {
			// push
		//}


	return result;
}