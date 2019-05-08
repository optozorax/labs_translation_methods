#include <vector>
#include <string>

template <class T>
struct Tree
{
	Tree* pred;
	Tree* left;
	Tree* right;
	T value;

	Tree(T value)
	{
		pred = nullptr;
		left = nullptr;
		right = nullptr;
		this->value = value;
	}
	Tree(T value, Tree* pred)
	{
		this->pred = pred;
		left = nullptr;
		right = nullptr;
		this->value = value;
	}

	~Tree()
	{
		//pred && pred->left == this ? pred->left = nullptr : pred->right = nullptr;
		if (left)
			delete left;
		if (right)
			delete right;
		//delete this;
	}

	static void SetL(Tree* tree, Tree* l)
	{
		tree->left = l;
	}
	static void SetR(Tree* tree, Tree* r)
	{
		tree->right = r;
	}
	
	static std::vector<T> pastorder(Tree* tree)
	{
		std::vector<T> a;
		if (tree->left) a = pastorder(tree->left);
		if (tree->right) a += pastorder(tree->right);
		a += value;
	}
};