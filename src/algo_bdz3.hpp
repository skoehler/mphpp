#pragma once

#include <vector>
#include <string>
#include <deque>

#include "randtools.hpp"
#include "unionfind2.hpp"
#include "graph3.hpp"
#include "bfs.hpp"
#include "algo.hpp"

/* Idea:
 * keep track of connected components with union find
 * keep track of nodes with degree 1 in each connected component
 * => if the number drops to zero, the graph is not acyclic?
 * => if the number never drops to uero, the graph is acyclic!
 *
 * Idea:
 * For each value of the 3 hash functions we need to store pairs of bits (values 0 to 3).
 * Instead of having one 32 bit integer value with 16 bit pairs,
 * we split 32 bit pairs as store them as two 32 bit integers.
 * When a hash value is determined, we need to compress it to the range [0,n).
 * For that, we need to check how many times the value 3 occurs.
 * That can be done by ANDing both 32 bit numbers and checking for 1 bits (popcount).
 *
 * for 10000 words:
 * 4049 * 3 * (2/8 + 2/32) = 3795.9375 bytes
 */

class AlgoBDZ3 {
private:
	using string = std::string;
	using size_t = Graph3::size_t;
	using vector = std::vector<size_t>;
	using deque = std::deque<size_t>;

public:
	double factor_init() {
		return 0.38;
	}
	double factor_inc() {
		return 1.02;
	}

	size_t findEdgeSeq(Graph3 &g, vector seq) {
		size_t m = 0;
		deque ones;
		size_t n = g.getN();
		for (size_t i = 0; i<n; i++) {
			if (g.degree(i) == 1) {
				ones.push_back(i);
			}
		}
		while (!ones.empty()) {
			size_t node = ones.front();
			ones.pop_front();
			if (g.degree(node) != 1) {
				// degree of node may have become zero by removing another edge
				continue;
			}

			size_t eidx = g.adjList(node).at(0);
			const Graph3::tuple_t &e = g.getEdge(eidx);
			seq.push_back(eidx);
			g.removeEdge(eidx);
			m++;
			for (int i=0; i<3; i++) {
				size_t n2 = e[i];
				if (g.degree(n2) == 1) {
					ones.push_back(n2);
				}
			}
		}

		return m;
	}


	bool run(randgen_t &randgen, const map_t &map,
			size_t maxlen, uint32_t n, size_t trials) {

		Graph3 g(3*n, map.size());
		// UnionFind2 uf(3*n);

		RandConst rsC0(0);
		RandConst rsC1(1);
		RandRange rs0_n(randgen, 0, n-1);
		RandRange rs1_n(randgen, 1, n-1);
		RandRange rs32Bit(randgen, 0, UINT32_MAX);

		// PreMult pre1(maxlen, rs1_n);
		// PreMult pre2(maxlen, rs1_n);
		// PreMult pre3(maxlen, rs1_n);
		// HashMultSum hf1(pre1, rsC0, rsC1);
		// HashMultSum hf2(pre2, rs0_n, rsC1);
		// HashMultSum hf3(pre3, rs0_n, rsC1);
		PreNone pre1;
		PreNone pre2;
		PreNone pre3;
		HashJenkinsOneAtATime hf1(pre1, rs32Bit);
		HashJenkinsOneAtATime hf2(pre2, rs32Bit);
		HashJenkinsOneAtATime hf3(pre3, rs32Bit);

		bool runagain = true;
		for (size_t i=0; runagain && i<trials; i++) {
			pre1.randomize();
			pre2.randomize();
			pre3.randomize();
			hf1.randomize();
			hf2.randomize();
			hf3.randomize();
			// uf.clear();
			g.clear();

			runagain = false;
			for (auto &x : map) {
				const string &key = x.first;
				uint32_t h1 = hf1.hash(key) % n + 0 * n;
				uint32_t h2 = hf2.hash(key) % n + 1 * n;
				uint32_t h3 = hf3.hash(key) % n + 2 * n;

				// size_t d1 = g.degree(h1);
				// size_t d2 = g.degree(h2);
				// size_t d3 = g.degree(h3);
				// size_t a = (d1 == 0 ? 1:0) + (d2 == 0 ? 1:0) + (d3 == 0 ? 1:0);
				// size_t s = (d1 == 1 ? 1:0) + (d2 == 1 ? 1:0) + (d3 == 1 ? 1:0);
				// uf.doUnion(h1, h2);
				// uf.doUnion(h1, h3);
				// size_t c = uf.adjustValue(h1, a, s);
				// if (c == 0) {
				// 	//TODO this almost never triggers, remove?
				// 	std::cout << "UF found cycle" << std::endl;
				// 	runagain = true;
				// 	break;
				// }
				// // std::cout << "adding (" << h1 << " " << h2 << " " << h3 << ") count " << c << std::endl;

				g.addEdge(h1, h2, h3);
			}

			if (runagain) {
				continue;
			}

			vector edgeSeq;
			size_t mc = findEdgeSeq(g, edgeSeq);
			if (mc != map.size()) {
				// std::cout << "findEdgeSeq found cycle" << std::endl;
				//FIXME this fails because of parallels, example edges:
				// (0,4,7), (1,4,7), (1,4,7)
				// throw std::runtime_error("internal error");
				runagain = true;
				continue;
			}

			return true;
		}
		return false;
	}
};


