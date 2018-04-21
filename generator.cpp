#include <iostream>
#include <memory>
#include <experimental\generator>
#include <stack>

using namespace std;
using namespace std::experimental;

class Array
{
public:
	void push(int data)
	{
		data_[ind_++] = data;
	}

	generator<int> print_co()
	{
		for (int i = 0; i < ind_; ++i)
		{
			co_yield data_[i];
		}
	}

private:
	int data_[10];
	int ind_ = 0;
};

template <typename T>
class Tree
{
private:
	struct Node
	{
		T data;
		shared_ptr<Node> left;
		shared_ptr<Node> right;
	};

	using NodePtr = shared_ptr<Node>;

public:
	void add(const T& data)
	{
		auto node = std::make_shared<Node>();
		node->data = data;
		if (root)
		{
			NodePtr parent;
			auto tmp = root;
			while (tmp)
			{
				parent = tmp;
				if (data < tmp->data)
				{
					tmp = tmp->left;
				}
				else
				{
					tmp = tmp->right;
				}
			}
			if (data < parent->data)
			{
				parent->left = node;
			}
			else
			{
				parent->right = node;
			}
		}
		else
		{
			root = node;
		}
	}

	generator<T> print_co()
	{
		NodePtr tmp = root;
		stack<NodePtr> s;
		while (true) {
			// First print the root node and then add left node
			while (tmp) 
			{
				co_yield tmp->data;
				s.push(tmp);
				tmp = tmp->left;
			}

			// check if Stack is emtpy, if yes, exit from everywhere
			if (s.empty()) {
				return;
			}
			// pop the element from the stack and go right to the tree
			tmp = s.top();
		    s.pop();
			tmp = tmp->right;
		}
	}

private:
	NodePtr root;
};

int main()
{
	Array a;
	a.push(90);
	a.push(100);

	for (int i : a.print_co())
	{
		cout << i << endl;
	}

	cout << "============" << endl;

	Tree<int> t;
	t.add(900);
	t.add(400);
	t.add(500);
	t.add(1200);

	for (auto i : t.print_co())
	{
		cout << i << endl;
	}

	return 0;
}
