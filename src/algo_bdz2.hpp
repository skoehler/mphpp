#pragma once

#include <vector>
#include <string>
#include <deque>

#include "randtools.hpp"
#include "unionfind.hpp"
#include "graph2.hpp"
#include "bfs.hpp"
#include "algo.hpp"

/* Idea:
 * keep track of connected components with union find
 * detect cycles
 *
 * Idea:
 * For each value of the 2 hash functions we need to store one bit.
 * For compressing the range of the hash function we store
 * 32 bit masks and 16 bit counters.
 *
 * for 10000 words:
 * 9371 * 2 * (1/8 + 6/32) = 5856.875 bytes
 */

class AlgoBDZ2 {
private:
	using string = std::string;
	using size_t = Graph2::size_t;
	using vector = std::vector<size_t>;
	using deque = std::deque<size_t>;

	// class ValueAssigner {
	// public:
	// 	vector values;

	// 	ValueAssigner(size_t n) : values(n, 0) {
	// 		// nothing
	// 	}

	// 	bool root(size_t r) {
	// 		(void)r;
	// 		return true;
	// 	}

	// 	bool normalEdge(size_t p, size_t c, const edge_t & edgeval) {
	// 		// child gets parent's value XOR edge value
	// 		values[c] = values[p] ^ edgeval;
	// 		return true;
	// 	}
	// 	void cycleEdge(size_t p, size_t c, const edge_t & edgeval) {
	// 		(void)p;
	// 		(void)c;
	// 		(void)edgeval;
	// 		throw std::runtime_error("BFS found cycle");
	// 	}
	// };

public:
	double factor_init() {
		return 0.9;
	}
	double factor_inc() {
		return 1.02;
	}

	bool run(randgen_t &randgen, const map_t &map,
			size_t maxlen, uint32_t n, size_t trials) {

		// RandConst rsC0(0);
		// RandConst rsC1(1);
		// RandConst rsC33(33);
		// RandRange rs0_n(randgen, 0, n-1);
		// RandRange rs1_n(randgen, 1, n-1);
		RandRange rs32Bit(randgen, 0, UINT32_MAX);

		// PreMult pre1(maxlen, rs1_n);
		// PreMult pre2(maxlen, rs1_n);
		// HashMultSum hf1(pre1, rs0_n, rsC1);
		// HashMultSum hf2(pre2, rs0_n, rsC1);
		PreNone pre1;
		PreNone pre2;
		HashJenkinsOneAtATime hf1(pre1, rs32Bit);
		HashJenkinsOneAtATime hf2(pre2, rs32Bit);
		// HashMultSum hf1(pre1, rs32Bit, rsC33);
		// HashMultSum hf2(pre2, rs32Bit, rsC33);

		{
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

		Graph2 g(2*n, map.size());
		for (auto &x : map) {
			const string &key = x.first;
			uint32_t h1 = hf1.hash(key) % n + 0;
			uint32_t h2 = hf2.hash(key) % n + n;
			g.addEdge(h1, h2);
		}

		// BFS bfs;
		// ValueAssigner vals(n);
		// bfs.visitAll(g, vals);


		return true;
	}
};


