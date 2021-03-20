#pragma once
#include <algorithm>
#include <tuple>

static const int b = 16;  // sizeof(key) * b = 64 (длина кеш линии)
static struct bleaf {
	bool leaf;
	uint8_t count;
	uint32_t keys[2 * b + 1];  // keys[2*b] не используется (он нужен для упрощения реализации)
};

static struct bnode : bleaf {
	bnode* kids[2 * b + 2];    // kids[2*b+1] не используется (он нужен для упрощения реализации)

	bool search(uint32_t key) const {
		size_t i = 0;
		while (i < count && keys[i] < key)
			i++;   // на коротких массивах линейный поиск быстрее бинарного
		if (i < count && keys[i] == key)
			return true;
		return !leaf && kids[i]->search(key);
	}

	std::pair<uint32_t, bnode*> insert_at_pos(size_t pos, uint32_t key, bnode* node_after_key) {
		memmove(keys + pos + 1, keys + pos, (count - pos) * sizeof(keys[0]));
		keys[pos] = key;  // "раздвигаем" keys и kids, чтобы вставить key и node_after_key
		if (!leaf) {
			memmove(kids + pos + 2, kids + pos + 1, (count - pos) * sizeof(kids[0]));
			kids[pos + 1] = node_after_key;
		}
		if (++count <= 2 * b)
			return { 0, nullptr };
		auto split = leaf ? (bnode*) new bleaf{ true } : new bnode{ false };
		count = split->count = b;  // в keys сейчас 2b+1 ключей; keys[b] пойдёт "наверх"
		std::copy(keys + b + 1, keys + 2 * b + 1, split->keys);
		if (!leaf)
			std::copy(kids + b + 1, kids + 2 * b + 2, split->kids);
		return { keys[b], split };  // ключ keys[b] не входит в узел и идёт "наверх"
	}

	std::pair<uint32_t, bnode*> insert(uint32_t key) {
		size_t pos = 0;
		while (pos < count && keys[pos] < key)
			pos++;  // на коротких массивах линейный поиск быстрее бинарного
		bnode* overflow_node = nullptr;
		if (!leaf)
			std::tie(key, overflow_node) = kids[pos]->insert(key);
		if (leaf || overflow_node != nullptr)
			return insert_at_pos(pos, key, overflow_node);
		return { 0, nullptr };
	}
};

class btree_simple {
	bnode* root = (bnode*) new bleaf{ true };
public:
	bool search(uint32_t key) { return root->search(key); }

	void insert(uint32_t key) {
		auto [okey, overflow_node] = root->insert(key);
		if (overflow_node != nullptr)  // увеличиваем высоту дерева "вверх"
			root = new bnode{ false, 1, { okey }, { root, overflow_node } };
	}
};