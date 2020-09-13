#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <bitset>
#include <deque>

#include "randtools.hpp"
#include "unionfind.hpp"
#include "algo.hpp"

/* Idea:
 * keep track of connected components with union find detect cycles
 *
 * Idea:
 * For each value of the 2 hash functions we need to store one bit.
 * For compressing the range of the hash function we store
 * 32 bit masks and 16 bit counters.
 *
 * Space for 10000 words:
 * 9371 * 2 * (1/8 + 6/32) = 5856.875 bytes
 */

class AlgoBDZ2 {
private:
	using string = std::string;
	using size_t = std::size_t;
	using vector = std::vector<size_t>;
	using deque = std::deque<size_t>;

	template<typename T>
	T removeBack(std::vector<T> &v) {
		T r = std::move(v.back());
		v.pop_back();
		return std::move(r);
	}

	template<typename T>
	void remove(std::vector<T> &v, const T &x) {
		auto xi = std::find(v.begin(), v.end(), x);
		if (xi != v.end()) {
			v.erase(xi);
		} else {
			throw std::runtime_error("element not found");
		}
	}

public:
	double factor_init() {
		return 0.9;
	}
	double factor_inc() {
		return 1.02;
	}

	bool run(randgen_t &randgen, const map_t &map,
			size_t maxlen, uint32_t n, size_t trials) {
		(void)maxlen;

		RandRange rs32Bit(randgen, 0, UINT32_MAX);

		PreNone pre1;
		PreNone pre2;
		HashJenkinsOneAtATime hf1(pre1, rs32Bit);
		HashJenkinsOneAtATime hf2(pre2, rs32Bit);

		{
			// find acyclic graph using union find
			UnionFind uf(2*n);
			bool runagain = true;
			for (size_t i=0; runagain && i<trials; i++) {
				pre1.randomize();
				pre2.randomize();
				hf1.randomize();
				hf2.randomize();
				uf.clear();

				runagain = false;
				for (auto &x : map) {
					const string &key = x.first;
					uint32_t h1 = hf1.hash(key) % n + 0;
					uint32_t h2 = hf2.hash(key) % n + n;
					bool cycle = uf.doUnion(h1, h2);
					if (cycle) {
						// cycle or parallel detected
						runagain = true;
						break;
					}
				}
			}
			if (runagain) {
				return false;
			}
		}

		typedef std::pair<uint32_t, uint32_t> edge_t;
		std::vector<edge_t> edges;
		vector eorder;

		{
			// build graph
			edges.reserve(map.size());
			std::vector<vector> adjList(2*n, vector());
			for (auto &x : map) {
				const string &key = x.first;
				uint32_t h1 = hf1.hash(key) % n + 0;
				uint32_t h2 = hf2.hash(key) % n + n;
				size_t eidx = edges.size();
				adjList[h1].push_back(eidx);
				adjList[h2].push_back(eidx);
				edges.push_back(edge_t(h1, h2));
			}

			// find leafs
			std::vector<size_t> leafNodes;
			for (size_t i = adjList.size(); i-- > 0; ) {
				if (adjList[i].size() == 1) {
					leafNodes.push_back(i);
				}
			}

			// iteratively remove leafs and incident edges
			eorder.reserve(edges.size());
			while (!leafNodes.empty()) {
				size_t leaf = removeBack(leafNodes);
				auto &ladj = adjList[leaf];
				if (ladj.empty()) {
					// degree might have already decreased to zero
					// example graph: a <--> b
					continue;
				}
				size_t eidx = removeBack(ladj);
				edge_t &e = edges[eidx];
				size_t p = (leaf != e.first) ? e.first : e.second;
				auto &padj = adjList[p];
				remove(padj, eidx);
				if (padj.size() == 1) {
					leafNodes.push_back(p);
				}
				// save order in which edges were removed
				eorder.push_back(eidx);
			}
		}

		// check whether all edges were removed
		if (eorder.size() != edges.size()) {
			throw std::runtime_error("internal error");
		}

		// assign 0/1 to nodes
		std::vector<bool> r(2*n, false);
		{
			std::vector<bool> leaf(2*n, true);
			for (size_t i = eorder.size(); i-- > 0; ) {
				size_t eidx = eorder[i];
				edge_t &e = edges[eidx];
				if (leaf[e.first]) {
					r[e.first] = r[e.second];
				} else {
					if (!leaf[e.second]) {
						throw std::runtime_error("internal error");
					}
					r[e.second] = !r[e.first];
				}
				leaf[e.first] = false;
				leaf[e.second] = false;
			}
		}

		{
			// sanity check
			std::vector<bool> used(2*n, false);
			for (const edge_t &e : edges) {
				bool r1 = r[e.first];
				bool r2 = r[e.second];
				bool s = (r1 != r2);
				size_t idx = s ? e.second : e.first;
				if (used[idx]) {
					throw std::runtime_error("sanity check failed");
				}
				used[idx] = true;
			}
		}

		return true;
	}
};


