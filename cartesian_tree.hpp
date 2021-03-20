#include <algorithm>

template<typename Tkey>
class cartesian_tree { //e-maxx's implementation
	struct item {
		Tkey key;
		int prior;
		item* l, * r;
		item() { }
		item(Tkey key, int prior) : key(key), prior(prior), l(nullptr), r(nullptr) { }
	};
	typedef item* pitem;

	pitem root;

	void split(pitem t, Tkey key, pitem& l, pitem& r) {
		if (!t)
			l = r = nullptr;
		else if (key < t->key)
			split(t->l, key, l, t->l), r = t;
		else
			split(t->r, key, t->r, r), l = t;
	}

	void insert(pitem& t, pitem it) {
		if (!t)
			t = it;
		else if (it->prior > t->prior)
			split(t, it->key, it->l, it->r), t = it;
		else
			insert(it->key < t->key ? t->l : t->r, it);
	}

public:
	cartesian_tree() { root = nullptr; }

	void insert(Tkey key, int prior) { insert(root, new item(key, prior)); }

	bool search(Tkey key) {
		auto node = root;
		while (node != nullptr) {
			if (node->key > key)
				node = node->l;
			else if (node->key < key)
				node = node->r;
			else
				return true;
		}
		return false;
	}
};
