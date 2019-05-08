#include <string>
#include <syn_analyzer.h>
#include <fstream>
#include <iostream>

class SynTable
{
public:
	SynTable(const std::string& path, Tables& tables);		//�����������

	void Next(Token);					//���������� ���������� ������

private:

	struct  Row							//������ ����������� �������
	{
		std::vector<string> term;
		int jmp;
		bool act;
		bool stk;
		bool ret;
		bool err;
	};

	string nameStr;						//��� ���������, ������� ��������������

	bool CheckTrashToken(Token token);	//�������� �������� �������
	int nstr;							//����� ������ � �������

	void Error(std::string);

	Token struc;
	Type type;							//��� ��������� �������
	Tables& tables;						//��� �������
	int state;							//������� ���������
	std::vector<int> stack_state;		//���� ��������� ���������
	std::vector<Row> syn_table;			//�������������� �������
};

//=============================================================================
//=============================================================================
//=============================================================================
//��������� ������� � ������ ��������� ��������
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
			auto b = tables.getStr(token);		//����������
			if (b[0] >= 'A' && b[0] <= 'Z') { f = true; break; }
			
		}
		if (i == "ind" && token.table == TABLE_IDENTIFIERS) { f = true; break; }
		if (i == "const" && token.table == TABLE_CONSTANTS) { f = true; break; }
		if (token == tables.find(i)) { f = true; break; }
	}

	int s;

	//���� ����� ���� � ������ ��������� ����
	if (f)
	{
		//���������� � ����
		if (syn_table[state].stk)
			stack_state.push_back(state + 1);

		//���������� ���������
		s = state;

		//a = b = c
		//b = (a+c)*(b-d)
		//b = a + c * d
		//b = a * c + d

		//������ ���
		switch (state)
		{
		case 12:	//���������� ��� ���������
			nameStr = tables.getStr(token);
			struc = tables.add(nameStr, TABLE_STRUCTURES);
			break;
		case 21:	//���������� ���� � ������� ���������
		{
			auto& arg = tables.get<Structure>(struc);
			Structure::StructElem elem;
			string name = tables.getStr(token);

			for (auto& i : arg.elems) //������ ���������� ���� � ���������
				if (i.name == name) Error("ind \"" + name + "\" was announced in struc \"" + nameStr + "\"");

			elem.name = name;
			elem.structToken = struc;
			elem.type = type;
			arg.elems.push_back(elem);
		}
			break;
		case 49: //���������� ����������
			if (type == TYPE_STRUCT)	//���� ��� ���������
			{
				auto& arg = tables.get<Structure>(struc);	//�� ���� �����, ������� ������ � �������
				for (auto& i : arg.elems)					//������ ��� �� ���������
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
			{	//������ ��� ����������
				auto& arg = tables.get<Identifier>(token);
				if (arg.type != TYPE_NONE) Error("\"" + tables.getStr(token) + "\" ind was announced"); //������ ���������� ��� ���� ���������
				arg.type = type;
			}
			break;
		case 28: //���������� ��� int
			type = TYPE_INT;
			break;
		case 29: //���������� ��� float
			type = TYPE_FLOAT;
			break;
		case 30: //���������� ��� ���������
			type = TYPE_STRUCT;
			struc = tables.find(tables.getStr(token), TABLE_STRUCTURES);
			break;
		case 23:
		case 43: //��������� ����������
		case 53:
			type = TYPE_NONE;
			break;
		default:
			break;
		}

		//����� �����, ����������� �� �����
		if (syn_table[state].ret)
		{
			//���� ������ �� ����� ������(�� ���� ��� �� ��������)
			if (!stack_state.size()) Error("stack going lim"); //������ ����� �� �����

			state = stack_state.back();
			stack_state.pop_back();
		}
		else
			state = syn_table[state].jmp;

		//���� ��� �� ���� �� ���� � ���� ������� ������
		if (!syn_table[s].act)
			Next(token);
	}
	else
		//���� ����� �� ������ ����� ��������� ����
		if (!syn_table[state].err)//� ��� �� ������
		{
			state++;		//��������� �� ������������
			Next(token);
		}
		else		//����� ��������� ��������� �� ������
		{
			std::string error = "instead "+ tables.getStr(token) + " expected: ";
			for (auto& i : syn_table[state].term)
				error += i;
			Error(error); //������ ��������� �� ��� �����
		}
}

//-----------------------------------------------------------------------------
std::vector<std::vector<string>> Analyzer(const std::vector<Token>& tokens, Tables& tables, string path){
	std::vector<std::vector<string>> result;
	
	SynTable syn_table(path, tables);

	for (auto i : tokens)
		//std::cout <<  tables.getStr(i) << std::endl;
		//������������ �� ������ ������
		syn_table.Next(i);
		//if (syn_table.Next(i)) {
			// push
		//}


	return result;
}