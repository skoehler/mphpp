#pragma once

#include <memory>
#include <unordered_map>

class Graph {
public:
	using size_t = std::size_t;
	using edge_t = std::uint64_t;

	using adj_t = std::pair<size_t, edge_t>;
	using adjList_t = std::vector<adj_t>;
//	using adjList_t = std::unordered_map<size_t, edge_t>;

private:
	const size_t n;
	std::unique_ptr<adjList_t[]> adjLists;

	void checkIndex(size_t x) const {
		if (x >= n) {
			throw std::runtime_error("invalid index");
		}
	}

public:
	Graph(size_t n) : n(n) {
		adjLists = std::make_unique<adjList_t[]>(n);
	}
	virtual ~Graph() {
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

	void addEdge(size_t i, size_t j, const edge_t &v) {
		checkIndex(i);
		checkIndex(j);
		adjLists[i].push_back(adj_t(j, v));
		adjLists[j].push_back(adj_t(i, v));
//		adjLists[i][j] = v;
//		adjLists[j][i] = v;
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

/**
 * A graph without parallels.
 */
class GraphSimple {
public:
	using size_t = std::size_t;
	using edge_t = std::uint64_t;

	using adjList_t = std::unordered_map<size_t, edge_t>;

private:
	const size_t n;
	std::unique_ptr<adjList_t[]> adjLists;

	void checkIndex(size_t x) const {
		if (x >= n) {
			throw std::runtime_error("invalid index");
		}
	}

public:
	GraphSimple(size_t n) : n(n) {
		adjLists = std::make_unique<adjList_t[]>(n);
	}
	virtual ~GraphSimple() {
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

	/**
	 * Add an edge if it does not already exist.
	 * @param i
	 * @param j
	 * @param v
	 * @return true iff the edge was added
	 */
	bool addEdge(size_t i, size_t j, const edge_t &v) {
		checkIndex(i);
		checkIndex(j);
		auto r1 = adjLists[i].insert(adjList_t::value_type(j, v));
		auto r2 = adjLists[j].insert(adjList_t::value_type(i, v));
		if (r1.second != r2.second) {
			throw std::runtime_error("internal error");
		}
		return r1.second;
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

