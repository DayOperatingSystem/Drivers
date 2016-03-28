#ifndef BINARYTREE_H
#define BINARYTREE_H

template<class T> class BinaryTree
{
public:
	struct Node
	{
		unsigned int id;
		T data;
		Node* left;
		Node* right;

		Node(T data, unsigned int id) : data(data), id(id), left(nullptr), right(nullptr) {}
	};

private:
	Node* root;

	T* doInsert(Node* parent, T data, unsigned int id)
	{
		// Insert at root
		if(!parent)
		{
			root = new Node(data, id);
			return &root->data;
		}

		if(id > parent->id)
		{
			// Found place to insert!
			if(!parent->right)
			{
				parent->right = new Node(data, id);
				return &parent->right->data;
			}

			return doInsert(parent->right, data, id);
		}
		else
		{
			// Found place to insert!
			if(!parent->left)
			{
				parent->left = new Node(data, id);
				return &parent->left->data;
			}

			return doInsert(parent->left, data, id);
		}
	}

	Node* doSearch(Node* parent, unsigned int id)
	{
		if(!parent)
			return nullptr;

		if(id == parent->id)
			return parent;
		if(id > parent->id)
			return doSearch(parent->right, id);
		else
			return doSearch(parent->left, id);
	}

	Node* findAndRemovePrevious(Node* node)
	{
		if(node->right && node->right->right)
			return findAndRemovePrevious(node->right);

		Node* retval = node->right;
		node->right = nullptr;

		return retval;
	}

	Node* doRemove(Node* node, unsigned int id)
	{
		if(node == nullptr)
			return nullptr;

		if(node->id == id)
		{
			if(node->left == nullptr && node->right == nullptr)
			{
				delete node;
				return nullptr;
			}
			else if(node->left == nullptr)
			{
				Node* retval = node->right;
				delete node;
				return retval;
			}
			else if(node->right == nullptr)
			{
				Node* retval = node->left;
				delete node;
				return retval;
			}
			else
			{
				Node* prev = findAndRemovePrevious(node->left);
				node->data = prev->data;
				delete prev;
				return node;
			}
		}

		if(id > node->id)
			node->right = doRemove(node->right, id);
		else
			node->left = doRemove(node->left, id);

		return node;
	}

	void doClear(Node* parent)
	{
		if(!parent)
			return;

		doClear(parent->left);
		doClear(parent->right);

		delete parent;
	}

public:

	BinaryTree()
	: root(nullptr) {}

	T* insert(T data, unsigned int id)
	{
		return doInsert(root, data, id);
	}

	T find(unsigned int id)
	{
		Node* node = doSearch(root, id);

		return (node) ? node->data : T();
	}

	void remove(unsigned int id)
	{
		root = doRemove(root, id);
	}

	void clear()
	{
		doClear(root);
	}
};

#endif // BINARYTREE_H
