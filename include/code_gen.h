#include <vector>
#include <string>
#include <tables.h>

std::string Code_gen(const std::vector<std::vector<Token>>& postfix, Tables& tables, std::vector<std::string>& ident, std::vector<std::string>& cons);