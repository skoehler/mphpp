#pragma once

#include <vector>
#include <string>

#include "randtools.hpp"
#include "unionfind.hpp"
#include "graph.hpp"
#include "bfs.hpp"
#include "algo.hpp"

class AlgoCHM {
private:
	using string = std::string;
	using edge_t = Graph::edge_t;
	using vector = std::vector<edge_t>;

	class ValueAssigner {
	public:
		vector values;

		ValueAssigner(size_t n) : values(n, 0) {
			// nothing
		}

		bool root(size_t r) {
			(void)r;
			return true;
		}

		bool normalEdge(size_t p, size_t c, const edge_t & edgeval) {
			// child gets parent's value XOR edge value
			values[c] = values[p] ^ edgeval;
			return true;
		}
		void cycleEdge(size_t p, size_t c, const edge_t & edgeval) {
			(void)p;
			(void)c;
			(void)edgeval;
			throw std::runtime_error("BFS found cycle");
		}
	};

public:
	double factor_init() {
		return 1.7;
	}
	double factor_inc() {
		return 1.05;
	}


	bool run(randgen_t &randgen, const map_t &map,
			size_t maxlen, uint32_t n, size_t trials) {

		// use factors that are a power of 2 plus/minus 1
		// this means the compiler can implement it with
		// a shift and an addition/subtration.
//		vector<uint32_t> rsFactorList;
//		for (uint32_t i=4; i >= 4; i *= 2) {
//			if (i-1 < n) {
//				rsFactorList.push_back(i-1);
//			}
//			if (i+1 < n) {
//				rsFactorList.push_back(i+1);
//			}
//		}

		RandConst rsC0(0);
		RandConst rsC1(1);
//		RandConst rsC33(33);
//		RandConst rsC5381(5381);
//		RandRange rs0_256(randgen, 0, 255);
		RandRange rs0_n(randgen, 0, n-1);
		RandRange rs1_n(randgen, 1, n-1);
//		RandPrime rp1_n(randgen, 1, n-1);
//		RandList  rsFactor(randgen, rsFactorList);

		PreMult pre1(maxlen, rs1_n);
		PreMult pre2(maxlen, rs1_n);
		HashMultSum hf1(pre1, rsC0, rsC1);
		HashMultSum hf2(pre2, rs0_n, rsC1);

//		PreXOR pre1(maxlen, rs0_256);
//		PreXOR pre2(maxlen, rs0_256);
//		HashMultSum hf1(pre1, rs1_n, rsFactor);
//		HashMultSum hf2(pre2, rs1_n, rsFactor);

		{
			UnionFind uf(n);
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
					uint32_t h1 = hf1.hash(key) % n;
					uint32_t h2 = hf2.hash(key) % n;
					bool circle = uf.doUnion(h1, h2);
					if (circle) {
						// cycle, parallel, or loop detected
						runagain = true;
						break;
					}
				}
			}
			if (runagain) {
				return false;
			}
		}

		Graph g(n);
		for (auto &x : map) {
			const string &key = x.first;
			uint32_t h1 = hf1.hash(key) % n;
			uint32_t h2 = hf2.hash(key) % n;
			g.addEdge(h1, h2, x.second);
		}

		BFS bfs;
		ValueAssigner vals(n);
		bfs.visitAll(g, vals);

		//TODO save working coefficients
		//TODO check!

		return true;
	}
};


