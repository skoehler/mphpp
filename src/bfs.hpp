#pragma once

#include <tuple>
#include <deque>
#include <vector>

#include "graph.hpp"

class BFS {
public:
	using size_t = std::size_t;

private:
	using qe_t = std::pair<size_t, size_t>;
	std::vector<bool> done;
	std::deque<qe_t> q;

	void clearDone(size_t n) {
		done.assign(n, false);
	}

	template <class G, class L>
	void visitNode(const G &g, size_t s, L &listener) {
		if (done[s]) {
			return;
		}
		if (!listener.root(s)) {
			return;
		}
		// root has no parent, so use invalid index n
		size_t p = g.getN();
		done[s] = true;
		q.clear();
		while (true) {
			for (auto &x : g.adjList(s)) {
				size_t neighbor = x.first;
				const Graph::edge_t &edge_val = x.second;
				if (neighbor == p) {
					// ignore edge to parent
					continue;
				}
				if (done[neighbor]) {
					listener.cycleEdge(s, neighbor, edge_val);
				} else {
					bool cont = listener.normalEdge(s, neighbor, edge_val);
					if (cont) {
						q.push_back(qe_t(neighbor, s));
						done[neighbor] = true;
					}
				}
			}
			if (q.empty()) {
				break;
			}
			std::tie(s, p) = q.front();
			q.pop_front();
		}
	}

public:
	template <class G, class L>
	void visitAll(const G &g, L &listener) {
		size_t n = g.getN();
		clearDone(n);
		for (size_t i=0; i<n; i++) {
			visitNode<G, L>(g, i, listener);
		}
	}

};

