#include <algorithm>  // Для компиляции нужно включить C++17
using namespace std;

const int b = 16; // sizeof(key) * b = 64 (длина кеш линии)
struct leaf {
	bool l;
	uint8_t n;
	int ks[2 * b + 1];
};

struct node : leaf {
	node* kids[2 * b + 2];

	bool find(int k) const {
		int i = 0;
		while (i < n && ks[i] < k) i++;
		return (i < n && ks[i] == k) || (!l && kids[i]->find(k));
	}

	pair<int, node*> insert(int k) {
		int i = 0;
		while (i < n && ks[i] < k) i++;
		if (l) return insert_at(i, k, 0);
		auto [p, x] = kids[i]->insert(k);
		if (!x) return { 0, 0 };
		return insert_at(i, p, x);
	}
	
private:
	pair<int, node*> insert_at(int i, int k, node* x) {
		memmove(ks + i + 1, ks + i, (n - i) * sizeof *ks);
		ks[i] = k;
		if (x) memmove(kids + i + 2, kids + i + 1, (n - i) * sizeof *kids);
		if (x) kids[i + 1] = x;
		if (++n <= 2 * b) return { 0, 0 };
		return split();
	}

	pair<int, node*> split() {
		auto x = l ? (node*) new leaf{true} : new node{false};
		n = x->n = b;
		memmove(x->ks, ks + b + 1, b * sizeof *ks);
		if (!l) memmove(x->kids, kids + b + 1, (b + 1) * sizeof *kids);
		return { ks[b], x };
	}
};

node* root = (node*) new leaf{true};

void insert(int k) {
	auto [p, x] = root->insert(k);
	if (x) root = new node{ false, 1, { p }, { root, x } };
}
