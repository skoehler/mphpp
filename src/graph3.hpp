#pragma once

#include <array>
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>

class Graph3 {
public:
	using size_t = std::size_t;
	using tuple_t = std::array<const size_t, 3>;

private:
	static const size_t eof = -1;

	using adjList_t = std::vector<size_t>;

	const size_t n;
	std::unique_ptr<adjList_t[]> adjLists;

	std::vector<tuple_t> edges;
//	size_t nodesWithDegree1 = 0;

	void checkIndex(size_t x) const {
		if (x >= getN()) {
			throw std::runtime_error("invalid index");
		}
	}

//	static void sortIndexes(size_t &a, size_t &b, size_t &c) {
//		if (b > c) {
//			std::swap(b, c);
//		}
//		if (a > b) {
//			std::swap(a, b);
//			if (b > c) {
//				std::swap(b, c);
//			}
//		}
//	}

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
	Graph3(size_t n, size_t m) : n(n) {
		adjLists = std::make_unique<adjList_t[]>(n);
		edges.reserve(m);
	}
	virtual ~Graph3() {
		// nothing
	}

	size_t getN() const {
		return n;
	}

//	size_t getN1() const {
//		return nodesWithDegree1;
//	}

	void clear() {
		for (size_t i=0; i<n; i++) {
			adjLists[i].clear();
		}
		edges.clear();
//		nodesWithDegree1 = 0;
	}

	void addEdge(size_t a, size_t b, size_t c) {
		checkIndex(a);
		checkIndex(b);
		checkIndex(c);
		//sortIndexes(a, b, c);
		tuple_t edge { a, b, c };
		size_t idx = edges.size();
		edges.push_back(edge);
		addEdgeIdx(adjLists[a], idx);
		addEdgeIdx(adjLists[b], idx);
		addEdgeIdx(adjLists[c], idx);
	}

	void removeEdge(size_t idx) {
		tuple_t &e = edges.at(idx);
		removeEdgeIdx(adjLists[e[0]], idx);
		removeEdgeIdx(adjLists[e[1]], idx);
		removeEdgeIdx(adjLists[e[2]], idx);
	}
	void restoreEdge(size_t idx) {
		tuple_t &e = edges.at(idx);
		//TODO edge should be added only if not already incident
		addEdgeIdx(adjLists[e[0]], idx);
		addEdgeIdx(adjLists[e[1]], idx);
		addEdgeIdx(adjLists[e[2]], idx);
	}

	const tuple_t &getEdge(size_t idx) {
		return edges.at(idx);
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


