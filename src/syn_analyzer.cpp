#include <string>
#include <syn_analyzer.h>
#include <fstream>
#include <iostream>
#include <optional>
#include <stack>

class SynTable
{
public:
	SynTable(const std::string& path, Tables& tables);		//�����������

	bool /*std::optional<vector<Token>>*/ Next(Token);					//���������� ���������� ������
	std::vector<Token>	ret;

	std::vector<string> warning;
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
	void Warning(std::string);

	bool treestart;						//����� ��������� � ������

	Token struc;
	Type type;							//��� ��������� �������
	Tables& tables;						//��� �������
	int state;							//������� ���������
	std::vector<int> stack_state;		//���� ��������� ���������
	std::vector<Row> syn_table;			//�������������� �������

	std::stack<Token>	oper;			//��������

	int GetPriorOper(Token token)		//�������� ��������� ��������
	{
		auto str = tables.getStr(token);
		if (str == "*")
			return 8;
		if (str == "+" || str == "-")
			return 7;
		if (str == "==" || str == "!=" || str == "<" || str == ">")
			return 6;
		if (str == "(")
			return 2;
		if (str == "=")
			return 1;
	}		
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

	treestart = false;

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

void SynTable::Warning(std::string str)
{
	warning.push_back("Warning(" + std::to_string(nstr) + "): " + str);
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

bool/*std::optional<vector<Token>>*/ SynTable::Next(Token token)
{
	if (CheckTrashToken(token)) return false;// std::nullopt;

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

	bool out = false;
	int s;

	//���� ����� ���� � ������ ��������� ����
	if (f)
	{
		//���������� � ����
		if (syn_table[state].stk)
			stack_state.push_back(state + 1);

		//���������� ���������
		s = state;

		//��������� �������� ���������
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
			if (type == TYPE_STRUCT)
				elem.nameStruct = nameStr;
			arg.elems.push_back(elem);
		}
			break;
		case 49: //���������� ����������
		{
			auto& arg = tables.get<Identifier>(token);
			if (type == TYPE_STRUCT)	//���� ��� ���������
			{
				auto& argStruc = tables.get<Structure>(struc);	//�� ���� �����, ������� ������ � �������
				for (auto& i : argStruc.elems)					//������ ��� �� ���������
				{
					auto tok = tables.find(tables.getStr(token) + "." + i.name);
					if (tok)
					{
						auto& argtok = tables.get<Identifier>(tok);
						argtok.type = i.type;
					}
				}
				arg.nameStruct = nameStr;
			}
			if (type != TYPE_NONE)
			{	//������ ��� ����������
				if (arg.type != TYPE_NONE) Error("\"" + tables.getStr(token) + "\" ind was announced"); //������ ���������� ��� ���� ���������
				arg.type = type;
			}
			else
				if (tables.get<Identifier>(token).type == TYPE_NONE) Error("\"" + tables.getStr(token) + "\" ind not announced"); //������ ���������� �� ����� ���������
				else type = tables.get<Identifier>(token).type, nameStr = tables.get<Identifier>(token).nameStruct;

			ret.push_back(token);
		}
			break;
			//������
		case 73:	// =
			treestart = true;
			oper.push(token);
			break;
			//���������
		case 88:   //id �����������
		{
			ret.push_back(token);
			auto& arg = tables.get<Identifier>(token);
			if (arg.type == TYPE_STRUCT) Error("Cannot cast type \"" + arg.nameStruct + "\" in expression");
		}
			break;
		case 89:	//���������
		{
			auto& arg = tables.get<Constant>(token);
			string name = tables.getStr(token);
			Type b;
			name.find(".") == -1 ? b = TYPE_INT : b = TYPE_FLOAT;
			if (type == TYPE_INT && b == TYPE_FLOAT) Warning("loss of accuracy is possible");
			arg.type = b;
			ret.push_back(token);
		}
			break;
		case 90: // (
			oper.push(token);
			break;
		case 92:// )
			Token top = oper.top();
			oper.pop();
			Token scob = tables.find("(");
			while (top != scob)
			{
				ret.push_back(top);
				top = oper.top();
				oper.pop();
			}
			break;
		case 93:// ����������� ���������
			if (type == TYPE_STRUCT) Error("Cannot cast expression to \"" + nameStr + "\"");
			break;
		case 64:
		case 65:
		case 66:
		case 67:	//��������
		case 68:
		case 69:
		case 70:
			while (oper.size() && GetPriorOper(oper.top()) >= GetPriorOper(token))
			{
				ret.push_back(oper.top());
				oper.pop();
			}
			oper.push(token);
			break;
		case 78:
		{
			//root->right = new Tree<Token>(token);
			//treeptr = root->right;
			ret.push_back(token);
			auto& arg = tables.get<Identifier>(token);
			if (type == TYPE_STRUCT && arg.type == TYPE_STRUCT && nameStr != arg.nameStruct) Error("Cannot cast type \"" + nameStr + "\" from \"" + arg.nameStruct + "\"");
			if (type == TYPE_STRUCT && arg.type != TYPE_STRUCT) Error("Cannot cast type \"" + nameStr + "\ from " + (arg.type == TYPE_INT ? "\" int\"" : "\" float\""));
			if (type != TYPE_STRUCT && arg.type == TYPE_STRUCT) Error("Cannot cast type \"" + type /*(type == TYPE_INT ? "int\ to" : "float\ to")*/ + arg.nameStruct  + "\"");
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
			nameStr = tables.getStr(token);
			struc = tables.find(nameStr , TABLE_STRUCTURES);
			break;
		case 23:
		case 46:
		case 43: //��������� ����������
		//case 53:
			type = TYPE_NONE;
		case 54:
			if (treestart)
			{
				treestart = false;
				while (oper.size())
				{
					ret.push_back(oper.top());
					oper.pop();

				}
				out = true;
			}
			else
				ret.clear();
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
			if (Next(token))
				out = true;
	}
	else
		//���� ����� �� ������ ����� ��������� ����
	if (!syn_table[state].err)//� ��� �� ������
	{
		state++;		//��������� �� ������������
		if (Next(token))
			out = true;
	}
	else		//����� ��������� ��������� �� ������
	{
		std::string error = "instead "+ tables.getStr(token) + " expected: ";
		for (auto& i : syn_table[state].term)
			error += i;
		Error(error); //������ ��������� �� ��� �����
	}

	return out;
}

//-----------------------------------------------------------------------------
std::vector<std::vector<Token>> Analyzer(const std::vector<Token>& tokens, Tables& tables, string path){
	std::vector<std::vector<Token>> result;
	
	SynTable syn_table(path, tables);

	for (auto i : tokens)
	{
		//std::cout << tables.getStr(i) << std::endl;
		//������������ �� ������ ������
		if (syn_table.Next(i))
		{
			result.push_back(syn_table.ret);
			syn_table.ret.clear();
		}
		//auto elem = syn_table.Next(i).value_or(vector<Token>());
	}

	for (auto& i : syn_table.warning)	//����� ��������������
		std::cout << i << endl;
	return result;
}