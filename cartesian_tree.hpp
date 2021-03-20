#pragma once
#include <algorithm>

template<typename Tkey>
class cartesian_tree { //e-maxx's implementation
	struct item {
		Tkey key;
		int prior;
		item* left, * right;
		item() { }
		item(Tkey key, int prior) : key(key), prior(prior), left(nullptr), right(nullptr) { }
	};
	typedef item* pitem;

	pitem root;

	void split(pitem node, Tkey key, pitem& left, pitem& right) {
		if (!node) {
			left = right = nullptr;
		} else if (key < node->key) {
			split(node->left, key, left, node->left);
			right = node;
		} else {
			split(node->right, key, node->right, right);
			left = node;
		}
	}

	void insert(pitem& node, pitem inode) {
		if (!node) {
			node = inode;
		} else if (inode->prior > node->prior) {
			split(node, inode->key, inode->left, inode->right);
			node = inode;
		} else {
			insert(inode->key < node->key ? node->left : node->right, inode);
		}
	}

public:
	cartesian_tree() { root = nullptr; }

	void insert(Tkey key, int prior) { insert(root, new item(key, prior)); }

	bool search(Tkey key) {
		for (auto node = root; node != nullptr; ) {
			if (node->key == key)
				return true;
			node = node->key > key ? node->left : node->right;
		}
		return false;
	}
};
