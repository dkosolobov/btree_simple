#pragma once
#include <stdint.h>
#include <algorithm>
#include <tuple>


template <typename Tkey, const int b = 16>
class btree_simple {
	struct bnode;
	struct bleaf {
		bool leaf;
		uint8_t count;
		Tkey keys[2 * b + 1];  // последний key не используется (для упрощения реализации)
	};
	struct bnode : public bleaf {
		bnode* kids[2 * b + 2]; // последний kid не используется (для упрощения реализации)
	};

	bnode* root;

	static size_t first_non_smaller_key(bnode* node, Tkey key) {
		size_t i = 0;
		while (i < node->count && node->keys[i] < key)
			i++;  // на коротком массиве linsearch быстрее binsearch
		return i;
	}

	static bool search(bnode* node, Tkey key) {
		size_t i = first_non_smaller_key(node, key);
		if (i < node->count && node->keys[i] == key)
			return true;
		return !node->leaf && search(node->kids[i], key);
	}

	static std::pair<Tkey, bnode*> insert_at_pos(bnode* node, size_t pos, Tkey key, bnode* node_after_key) {
		memmove(node->keys + pos + 1, node->keys + pos, (node->count - pos) * sizeof(Tkey));
		node->keys[pos] = key;  // "раздвигаем" keys и kids, чтобы вставить key и node_after_key
		if (!node->leaf) {
			memmove(node->kids + pos + 2, node->kids + pos + 1, (node->count - pos) * sizeof(bnode*));
			node->kids[pos + 1] = node_after_key;
		}
		node->count++;
		if (node->count <= 2 * b)
			return { Tkey(), nullptr };  // вставка закончена, "наверх" ничего не идёт

		auto split = node->leaf ? reinterpret_cast<bnode*>(new bleaf{ true }) : new bnode{ false };
		split->count = node->count = b;  // в node сейчас 2b+1 ключей; node->keys[b] "пойдёт наверх"
		std::copy(node->keys + b + 1, node->keys + 2 * b + 1, split->keys);
		if (!node->leaf)
			std::copy(node->kids + b + 1, node->kids + 2 * b + 2, split->kids);
		return { node->keys[b], split };  // ключ node->keys[b] не входит в node и "идёт наверх"
	}

	static std::pair<Tkey, bnode*> insert(bnode* node, Tkey key) {
		size_t pos = first_non_smaller_key(node, key);
		bnode* overflow_node = nullptr;
		if (!node->leaf)
			std::tie(key, overflow_node) = insert(node->kids[pos], key);
		if (node->leaf || overflow_node != nullptr)
			return insert_at_pos(node, pos, key, overflow_node);
		return { Tkey(), nullptr };
	}

public:
	btree_simple() { root = reinterpret_cast<bnode*>(new bleaf{true}); }

	bool search(Tkey key) { return search(root, key); }

	void insert(Tkey key) {
		auto [okey, overflow_node] = insert(root, key);
		if (overflow_node != nullptr)  // увеличиваем высоту дерева "вверх"
			root = new bnode{ false, 1, { okey }, { root, overflow_node } };
	}
};