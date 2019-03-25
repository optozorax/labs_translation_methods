#include <iostream>
#include <tables.h>

int main() {
	Tables t;

	std::cout << "find(\"{\") = " << t.find("{") << std::endl;
	std::cout << "find(\"`\") = " << t.find("`") << std::endl;
	std::cout << "find(\"r\") = " << t.find("r") << std::endl;
	std::cout << "add(\"r\", TABLE_IDENTIFIERS)" << std::endl;

	t.add("r", TABLE_IDENTIFIERS);
	t.add("5", TABLE_CONSTANTS);
	t.add("A", TABLE_STRUCTURES);

	std::cout << std::endl << "After adding these values: " << std::endl;
	std::cout << "find(\"r\") = " << t.find("r") << std::endl;
	std::cout << "find(\"5\") = " << t.find("5") << std::endl;
	std::cout << "find(\"A\") = " << t.find("A") << std::endl;

	auto& r1 = t.get<Identifier>(t.find("r"));
	r1.type = TYPE_FLOAT;

	auto& r2 = t.get<Constant>(t.find("5"));
	r2.value.i = 5;

	auto& r3 = t.get<Structure>(t.find("A"));
	r3.elems.push_back({});
	r3.elems.push_back({});
	r3.elems.push_back({});

	std::cout << std::endl << "After changing values: " << std::endl;
	std::cout << "type of r: " << t.get<Identifier>(t.find("r")).type << std::endl;
	std::cout << "value of 5: " << t.get<Constant>(t.find("5")).value.i << std::endl;
	std::cout << "field count of A: " << t.get<Structure>(t.find("A")).elems.size() << std::endl;

	system("pause");
}