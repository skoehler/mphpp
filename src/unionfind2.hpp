#pragma once

#include <memory>

class UnionFind2 {
public:
	using size_t = std::size_t;
private:
	struct node_t {
		/**
		 * Size of the tree rooted at this node.
		 */
		size_t s;
		/**
		 * Value stored for the tree root.
		 */
		size_t v;
		/**
		 * Index of the parent node.
		 * Roots have themselves as a parent.
		 */
		size_t p;
	} ;

	const size_t n;
	std::unique_ptr<node_t[]> nodes;

	void checkIndex(size_t x) const {
		if (x >= n) {
			throw std::runtime_error("invalid index");
		}
	}

public:
	UnionFind2(size_t n) : n(n) {
		nodes = std::make_unique<node_t[]>(n);
		clear();
	}

	virtual ~UnionFind2() {
		// nothing
	}

	size_t getN() const {
		return n;
	}

	void clear() {
		for (size_t i = 0; i<n; i++) {
			node_t &n = nodes[i];
			n.s = 1;
			n.v = 0;
			n.p = i;
		}
	}

	size_t findIdentity(size_t i) const {
		checkIndex(i);
		// find root
		size_t r = i;
		while (nodes[r].p != r) {
			r = nodes[r].p;
		}
		// path compression
		while (nodes[i].p != r) {
			size_t p = nodes[i].p;
			nodes[i].p = r;
			i = p;
		}
		return r;
	}

	size_t adjustValue(size_t i, size_t add, size_t sub) {
		size_t ri = findIdentity(i);
		auto &ni = nodes[ri];
		return ni.v = ni.v + add - sub;
	}

	bool doUnion(size_t i, size_t j) {
		size_t ri = findIdentity(i);
		size_t rj = findIdentity(j);
		if (ri == rj) {
			return true;
		}
		auto &ni = nodes[ri];
		auto &nj = nodes[rj];
		// union by size (smaller tree is added to bigger tree)
		if (ni.s >= nj.s) {
			ni.s += nj.s;
			ni.v += nj.v;
			nj.p = ri;
		} else {
			nj.s += ni.s;
			nj.v += ni.v;
			ni.p = rj;
		}
		return false;
	}
};



