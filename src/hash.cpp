#include <hash.h>

using namespace std;

Hash::Hash()
{
	hashFunc = [](string str) 
	{
		int sum = 0;
		for (size_t i = 0; i < str.size(); i++)
			sum += str[i];
		return sum;
	};

	Add(Keywords, "int");
	Add(Keywords, "float");
	Add(Keywords, "struct");

	Add(Delimiters, " ");
	Add(Delimiters, ";");
	Add(Delimiters, "\n");
	Add(Delimiters, "{");
	Add(Delimiters, "}");

	Add(Operation, "=");
	Add(Operation, "+");
	Add(Operation, "-");
	Add(Operation, "*");
	Add(Operation, "==");
	Add(Operation, "!=");
	Add(Operation, "<");
	Add(Operation, ">");
}


Hash::~Hash()
{
}

Hash::Tocken Hash::Find(string name)
{
	Tocken b;
	for (int i = 0; i < n; i++)
	{
		b = Check(i, name);
		if (b)
			return b;
	}
	return Hash::Tocken{ -1, -1, -1 };
}

//!!!!!!УСЛОВИЕ!!!!!!!
//структуры с большой буквы
//индентификаторы с маленькой
//констатнты с цифы или ковычки
bool Hash::Push(string b)
{
	if (!b.size())
		return 0;
	int n;
	if (b[0] == '\"' || b[0] == '\'' || b[0] >= 0 && b[0] <= 9)
		n = Constant;
	else
	{
		if (b[0] >= 'A' && b[0] <= 'Z')
			n = Structures;
		else
			if (b[0] >= 'a' && b[0] <= 'z')
				n = Indintificator;
			else
				return 0;
	}

	if (!Check(n, b))
	{
		Add(n, b);
		return 1;
	}
	else
		return 0;
}

bool Hash::SetArgIndint(Tocken tocken, Indint ind)
{
	if (tocken) {
		string name = indintificator[tocken.nString][tocken.nPos].name;
		ind.name = name;
		indintificator[tocken.nString][tocken.nPos] = ind;
		return true;
	}
	else
		return false;
}

bool Hash::SetArgConst(Tocken tocken, Const con)
{
	if (tocken) {
		string name = constant[tocken.nString][tocken.nPos].name;
		con.name = name;
		constant[tocken.nString][tocken.nPos] = con;
		return true;
	}
	else
		return false;
}

bool Hash::SetArgStruct(Tocken tocken, Struct str)
{
	if (tocken) {
		string name = structures[tocken.nString][tocken.nPos].name;
		str.name = name;
		structures[tocken.nString][tocken.nPos] = str;

		return true;
	}
	else
		return false;
}

Hash::Indint Hash::GetArgIndint(Tocken  name)
{
	if (name)
		return indintificator[name.nString][name.nPos];

	Indint a("-1");
	return a;
}

Hash::Const Hash::GetArgConst(Tocken  name)
{
	if (name)
		return constant[name.nString][name.nPos];

	Indint a("-1");
	return a;
}

Hash::Struct Hash::GetArgStruct(Tocken  name)
{
	if (name)
		return structures[name.nString][name.nPos];

	Indint a("-1");
	return a;
}

void Hash::SetHeshFunc(function<int(string)> func)
{
	hashFunc = func;
}

void Hash::Add(int nTable, string name)
{
	switch (nTable)
	{
	case 0:
		if(!Check(0, name))
			keywords[hashFunc(name) % Nkw].push_back(name);
		break;
	case 1:
		if(!Check(1, name))
			operation[hashFunc(name) % No].push_back(name);
		break;
	case 2:
		if(!Check(2, name))
			delimiters[hashFunc(name) % Nd].push_back(name);
		break;
	case 3:
		if (!Check(3, name))
		{
			Indint a(name);
			indintificator[hashFunc(name) % Ni].push_back(a);
		}
		break;
	case 4:
		if (!Check(4, name))
		{
			Const a(name);
			constant[hashFunc(name) % Nc].push_back(a);
		}
		break;
	case 5:
		if (!Check(5, name))
		{
			Struct a(name);
			structures[hashFunc(name) % Ns].push_back(a);
		}
		break;
	}
}

Hash::Tocken Hash::Check(int nTable, string name)
{
	switch (nTable)
	{
	case 0:
	{
		int b = hashFunc(name) % Nkw;
		for (int i = 0; i < keywords[b].size(); i++)
		{
			if (name == keywords[b][i])
				return Hash::Tocken{ 0, b, i };
		}
		break;
	}
	case 1:
	{
		int b = hashFunc(name) % No;
		for (int i = 0; i < operation[b].size(); i++)
		{
			if (name == operation[b][i])
				return Hash::Tocken{ 1, b, i };
		}
		break;
	}
	case 2:
	{
		int b = hashFunc(name) % Nd;
		for (int i = 0; i < delimiters[b].size(); i++)
		{
			if (name == delimiters[b][i])
				return Hash::Tocken{ 2, b, i };
		}
		break;
	}
	case 3:
	{
		int b = hashFunc(name) % Ni;
		for (int i = 0; i < indintificator[b].size(); i++)
		{
			if (name == indintificator[b][i].name)
				return Hash::Tocken{ 3, b, i };
		}
		break;
	}
	case 4:
	{
		int b = hashFunc(name) % Nkw;
		for (int i = 0; i < constant[b].size(); i++)
		{
			if (name == constant[b][i].name)
				return Hash::Tocken{ 4, b, i };
		}
		break;
	}
	case 5:
	{
		int b = hashFunc(name) % Nkw;
		for (int i = 0; i < structures[b].size(); i++)
		{
			if (name == structures[b][i].name)
				return Hash::Tocken{ 5, b, i };
		}
		break;
	}
	default:
		return Hash::Tocken{-1, -1, -1};
	}
	return Hash::Tocken{ -1, -1, -1 };
}

Hash::Const::operator Hash::Indint()
{
	Indint a(this->name);
	return a;
}

Hash::Const::operator Hash::Struct()
{
	Struct a(this->name);
	return a;
}

Hash::Const::operator bool()
{
	if (type == -1)
		return 0;
	return 1;
}

Hash::Const::Const(string name)
{
	this->name = name;
	this->type = -1;
	this->value = -1;
}

Hash::Indint::operator Hash::Const()
{
	Const a(this->name);
	return a;
}

Hash::Indint::operator Hash::Struct()
{
	Struct a(this->name);
	return a;
}

Hash::Indint::operator bool()
{
	if (type == -1)
		return 0;
	return 1;
}

Hash::Indint::Indint(string name)
{
	this->name = name;
	this->type = -1;
	this->value.i = -1;
	isInitialized = false;
}

Hash::Struct::operator Hash::Indint()
{
	Indint a(this->name);
	return a;
}

Hash::Struct::operator Hash::Const()
{
	Const a(this->name);
	return a;
}

Hash::Struct::operator bool()
{
	if (this->nInt && this->nFloat)
		return 1;
	else
		return 0;
}

Hash::Struct::Struct(string name)
{
	this->name = name;
	this->nFloat = 0;
	this->nInt = 0;
}

Hash::Tocken::operator bool()
{
	if (nTable == -1 || nString == -1 || nPos == -1)
		return false;
	return true;
}
