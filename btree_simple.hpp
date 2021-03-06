#pragma once         // Для компиляции включите C++17:
#include <algorithm> // - VS: флаг /std:c++17 в Project > Properties > C/C++ > Language > C++ Language Standard 
#include <tuple>     // - clang/gcc: флаг -std=c++17 или -std=c++1z

static const int b = 16;  // sizeof(key) * b = ~64 (длина кеш линии)
struct bleaf {
	bool leaf;
	uint8_t count;
	int keys[2 * b + 1];  // keys[2*b] не используется, но нужен для упрощения реализации
};

struct bnode : bleaf {
	bnode* kids[2 * b + 2];    // kids[2*b+1] не используется, но нужен для упрощения реализации

	bool find(int key) const {
		size_t i = 0;
		while (i < count && keys[i] < key)
			i++;   // на коротких массивах линейный поиск быстрее бинарного
		return (i < count && keys[i] == key) || (!leaf && kids[i]->find(key));
	}

	std::pair<int, bnode*> insert(int key) {
		size_t i = 0;
		while (i < count && keys[i] < key)
			i++;  // на коротких массивах линейный поиск быстрее бинарного
		if (leaf)
			return insert_at(i, key, nullptr);
		auto [overkey, overflow] = kids[i]->insert(key);
		if (overflow == nullptr)
			return { 0, nullptr };
		return insert_at(i, overkey, overflow);
	}

private:
	std::pair<int, bnode*> insert_at(size_t i, int key, bnode* key_node) {
		memmove(keys + i + 1, keys + i, (count - i) * sizeof *keys);
		keys[i] = key;  // раздвигаем keys и kids, чтобы вставить key и key_node
		if (key_node != nullptr) {
			memmove(kids + i + 2, kids + i + 1, (count - i) * sizeof *kids);
			kids[i + 1] = key_node;
		}
		if (++count <= 2 * b)
			return { 0, nullptr };
		return split();
	}

	std::pair<int, bnode*> split() {  // в keys 2b+1 ключей, keys[b] - медиана
		auto split = leaf ? (bnode*) new bleaf{true} : new bnode{false};
		count = split->count = b;
		std::copy(keys + b + 1, keys + 2 * b + 1, split->keys);
		if (!leaf)
			std::copy(kids + b + 1, kids + 2 * b + 2, split->kids);
		return { keys[b], split };  // keys[b] поднимается на уровень выше
	}
};

class btree_simple {
	bnode* root = (bnode*) new bleaf{true};
public:
	bool find(int key) const { return root->find(key); }

	void insert(int key) {
		auto [overkey, overflow] = root->insert(key);
		if (overflow != nullptr)  // увеличиваем высоту дерева "вверх"
			root = new bnode{ false, 1, { overkey }, { root, overflow } };
	}
};
