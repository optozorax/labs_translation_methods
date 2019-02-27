#pragma once

#include <vector>
#include <string>
#include <functional>

using namespace std;

class Hash
{
public:
	Hash();
	~Hash();

	struct Indint;
	struct Const;
	struct Struct;

	struct Tocken
	{
		int nTable;						//Номер таблицы
		int nString;					//Строка в хэш-таблице
		int nPos;						//Порядок в хэш-таблице

		operator bool();
	};

	struct Indint						//Аргументы у индинтификатора
	{
		string name;					//Имя индентификатора
		union
		{
			float f;
			int i;
		} value;
		int type;						//0 - int, 1 - float
		bool isInitialized;
		
		operator Hash::Const();
		operator Hash::Struct();
		operator bool();

		Indint(string name);			//Конструктор
	};

	struct Const						//Аргументы у константы
	{
		string name;
		int type;
		int value;

		operator Hash::Indint();
		operator Hash::Struct();
		operator bool();
		Const(string name);				//Конструктор
	};
	//Разделил чтобы можно было задавать разные индентификаторы

	struct Struct
	{
		string name;					//Название структуры
		int nInt;						//Колчиество целых чисел
		int nFloat;						//Количество вещественных чисел
		vector<string> valueName;		//Названия индинтификаторов в структуре
		vector<int> valueI;				//Динамический массив целых чисел(можно вектор сделать)
		vector<float> valueF;			//Динамический массив вещественныз чисел

		operator Hash::Indint();
		operator Hash::Const();
		operator bool();
		Struct(string name);
	};

	Tocken Find(string a);				//Поиск
	bool Push(string b);				//Добавление элемента в динамическую таблицу

	template<class T>
	bool SetArg(Tocken tocken, T arg)					//Добавдение аргументов
	{
		if (is_same<T, Hash::Indint>::value)
			return SetArgIndint(tocken, arg);
		if (is_same<T, Hash::Const>::value)
			return SetArgConst(tocken, arg);
		if (is_same<T, Hash::Struct>::value)
			return SetArgStruct(tocken, arg);

		return false;
	}

	template<class T>
	T GetArg(Tocken tocken)				//Получение аргумента
	{
		if (is_same<T, Hash::Indint>::value)
			return GetArgIndint(tocken);
		if (is_same<T, Hash::Const>::value)
			return GetArgConst(tocken);
		if (is_same<T, Hash::Struct>::value)
			return GetArgStruct(tocken);

		return T(0);
	}

	void SetHeshFunc(function<int(string)> func);		//Задание Хэш-функции

private:
	static const int Nkw = 10;			//Количество строк в хэш-таблице ключевых слов
	static const int No = 15;			//Количество строк в хэш-таблице операций
	static const int Nd = 10;			//Количество строк в хэш-таблице разделителкй
	static const int Ni = 255;			//Количество строк в хэш-таблице индинтификаторов
	static const int Nc = 63;			//Количество строк в хэш-таблице констант
	static const int Ns = 15;			//Количество строк в хэш-таблице структур
	static const int n = 6;				//Количество хэш-таблиц

	enum Table
	{
		Keywords = 0,
		Operation = 1,
		Delimiters = 2,
		Indintificator = 3,
		Constant = 4,
		Structures = 5
	};

	bool SetArgIndint(Tocken tocken, Indint ind);		//Добавление аргументов у индинтификаторов
	bool SetArgConst(Tocken tocken, Const con);		//Добавление аргументов у констант
	bool SetArgStruct(Tocken tocken, Struct str);		//Добавление аргументов у структур

	Indint GetArgIndint(Tocken name);	//Получить аргументы у индентификатора
	Const GetArgConst(Tocken name);		//Получить аргументы у константы
	Struct GetArgStruct(Tocken name);	//Получить аргументы у структуры

	void Add(int nTable, string name);		//Добавление элемента в таблицу
	Tocken Check(int nTable, string name);	//Проверка наличия элемента в таблице

	vector<string> keywords[Nkw];			//Ключевые слова
	vector<string> operation[No];			//Знаки операции
	vector<string> delimiters[Nd];			//Разделители
	vector<Indint> indintificator[Ni];		//Идентификаторы
	vector<Const> constant[Nc];				//Константы
	vector<Struct> structures[Ns];			//Структуры

	function<int(string)> hashFunc;		//Хэш функция
};