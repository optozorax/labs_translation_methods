#include <string>
#include <code_gen.h>
#include <fstream>
#include <iostream>
#include <optional>
#include <stack>

class CodeGen
{
public:
	CodeGen(Tables& tables);		//Конструктор

	std::string DataCode(std::vector<std::string>& ident);
	std::string CreateCode(std::vector<Token> input);
	std::string StartCode();
	std::string EndCode();
private:
	Tables& tables;						//Хэш таблицы	


	std::string PrintLastStekElem(std::vector<Token>& stack);
	void LieAdd(std::vector<Token>& stack);
};

//=============================================================================
//=============================================================================
//=============================================================================
CodeGen::CodeGen(Tables& tables) : tables(tables) {

}

std::string CodeGen::DataCode(std::vector<std::string>& ident)
{
	std::string str = ".data\n\ttmp_var	dd ?\n";
	
	for (auto& i : ident)
	{
		auto t = tables.find(i);
		auto arg = tables.get<Identifier>(t);
		if (arg.type != TYPE_STRUCT)
		{
			str += "\t" + i + "\t";
			if (arg.type == TYPE_FLOAT) str += "real8 ?\n";
			if (arg.type == TYPE_INT) str += "dd ?\n";
		}
	}
	
	str += "\n.code\n\tSTART:\n\tfinit\n\n";

	return str;
}

std::string CodeGen::CreateCode(std::vector<Token> input)
{
	string str = ";<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
	
	vector<Token> stack;

	for(auto& i : input)
	{
		if (i.table == TABLE_IDENTIFIERS || i.table == TABLE_CONSTANTS)
			stack.push_back(i);
		if (i.table == TABLE_OPERATIONS)
		{
			string name = tables.getStr(i);
			if (name == "=")
			{
				Token right = stack[stack.size() - 1];
				str += PrintLastStekElem(stack);

				Token left = stack[stack.size() - 1]; stack.pop_back();
				auto arg = tables.get<Identifier>(left);

				if (arg.type == TYPE_STRUCT)
				{
					string namel = tables.getStr(left);
					string namer = tables.getStr(right);
					auto args = tables.get<Structure>(tables.find(arg.nameStruct));
					for (auto& elem : args.elems)
					{
						//auto& aaa = tables.get<Identifier>(tables.find(namel + "." + elem.name));
						//aaa.isInitialized = true;
						//aaa.type = elem.type;
						if (elem.type == TYPE_FLOAT)
						{
							if (tables.get<Identifier>(tables.find(namer + "." + elem.name)).isInitialized)
								str += "\tfld\t" + namer + "." + elem.name + "\n";
							else
								str += "\tfldz\n";
							str += "\tfstp\t" + namel + "." + elem.name + "\n";
						}
						if (elem.type == TYPE_INT)
						{
							if (tables.get<Identifier>(tables.find(namer + "." + elem.name)).isInitialized)
								str += "\tfild\t" + namer + "." + elem.name + "\n";
							else
								str += "\tfldz\n";
							str += "\tfistp\t" + namel + "." + elem.name + "\n";
						}
					}


					if(stack.size() != 0)
						stack.push_back(right);
				}
				else
				{
					if (stack.size() == 0)
					{
						if (arg.type == TYPE_INT) str += "\tfistp\t" + tables.getStr(left) + "\n";
						if (arg.type == TYPE_FLOAT) str += "\tfstp\t" + tables.getStr(left) + "\n";
					}
					else
					{
						if (arg.type == TYPE_INT) str += "\tfist\t" + tables.getStr(left) + "\n";
						if (arg.type == TYPE_FLOAT) str += "\tfst\t" + tables.getStr(left) + "\n";

						LieAdd(stack);
					}
				}
				continue;
			}

			for (size_t k = 0; k < 2; k++)
				str += PrintLastStekElem(stack);

			LieAdd(stack);

			if (name == "+") str += "\tfadd\n";
			if (name == "-") str += "\tfsub\n";
			if (name == "*") str += "\tfmul\n";
		}
	}

	str += ";>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n";
	return str;
}

void CodeGen::LieAdd(std::vector<Token>& stack)
{
	Token a;
	a.table = TABLE_NULL;
	a.pos.line = 0;
	a.pos.pos = 0;
	stack.push_back(a);
}

std::string CodeGen::PrintLastStekElem(std::vector<Token>& stack)
{
	std::string str;
	Token tok = stack[stack.size() - 1]; stack.pop_back();
	if (tok.table == TABLE_IDENTIFIERS)
	{
		auto arg = tables.get<Identifier>(tok);
		if (arg.type == TYPE_INT) str += "\tfild\t" + tables.getStr(tok) + "\n";
		if (arg.type == TYPE_FLOAT) str += "\tfld\t" + tables.getStr(tok) + "\n";
	}
	if (tok.table == TABLE_CONSTANTS)
	{
		auto arg = tables.get<Constant>(tok);
		if (arg.type == TYPE_INT) str += "\tfild\t" + tables.getStr(tok) + "\n";
		if (arg.type == TYPE_FLOAT) str += "\tfld\t" + tables.getStr(tok) + "\n";
	}
	return str;
}

std::string CodeGen::StartCode()
{
	return ".386\n.MODEL FLAT, STDCALL\n\nEXTRN	ExitProcess@4:NEAR\n";
}

std::string CodeGen::EndCode()
{
	return "\tCALL ExitProcess@4\nEND START";
}

//-----------------------------------------------------------------------------
string Code_gen(const std::vector<std::vector<Token>>& postfix, Tables& tables, std::vector<std::string>& ident){
	CodeGen gen(tables);

	std::string code = gen.StartCode();
	code += gen.DataCode(ident);

	for (auto& i : postfix)
		code += gen.CreateCode(i);

	code += gen.EndCode();

	return code;
}