#pragma once

#include <array>
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>

class Graph2 {
public:
	using size_t = std::size_t;

private:
	static const size_t eof = -1;

	using adjList_t = std::vector<size_t>;

	const size_t n;
	std::unique_ptr<adjList_t[]> adjLists;

	void checkIndex(size_t x) const {
		if (x >= getN()) {
			throw std::runtime_error("invalid index");
		}
	}

	void addEdgeIdx(adjList_t &adj, size_t idx) {
		adj.push_back(idx);
//		switch (adj.size()) {
//		case 1:
//			nodesWithDegree1++;
//			break;
//		case 2:
//			nodesWithDegree1--;
//			break;
//		}
	}
	static void removeEdgeIdx(adjList_t &x, size_t idx) {
		auto y = std::find(x.begin(), x.end(), idx);
		if (y != x.end()) {
			x.erase(y);
		} else {
			throw std::runtime_error("remove non-incident edge");
		}
	}

public:
	/**
	 *
	 * @param n number of nodes
	 * @param m number of edges
	 */
	Graph2(size_t n, size_t m) : n(n) {
		adjLists = std::make_unique<adjList_t[]>(n);
	}
	virtual ~Graph2() {
		// nothing
	}

	size_t getN() const {
		return n;
	}

	void clear() {
		for (size_t i=0; i<n; i++) {
			adjLists[i].clear();
		}
	}

	void addEdge(size_t a, size_t b) {
		checkIndex(a);
		checkIndex(b);
		addEdgeIdx(adjLists[a], b);
		addEdgeIdx(adjLists[b], a);
	}

	void removeEdge(size_t a, size_t b) {
		//TODO implement
	}

	const adjList_t & adjList(size_t i) const {
		checkIndex(i);
		return adjLists[i];
	}

	size_t degree(size_t i) const {
		checkIndex(i);
		return adjLists[i].size();
	}
};


