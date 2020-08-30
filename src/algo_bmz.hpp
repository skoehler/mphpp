#pragma once

#include <vector>
#include <string>

#include "randtools.hpp"
#include "unionfind.hpp"
#include "graph.hpp"
#include "bfs.hpp"
#include "algo.hpp"

class AlgoBMZ {
private:
	using string = std::string;
	using edge_t = GraphSimple::edge_t;
	using vector = std::vector<edge_t>;
	using vectorb = std::vector<bool>;

	class CoreFinder {
	private:
		GraphSimple &g;
		vectorb &core;
	public:
		size_t nonCoreEdgesTimes2 = 0;

		CoreFinder(GraphSimple &g, vectorb &core) : g(g), core(core) {
			core.assign(g.getN(), true);
		}

		bool root(size_t r) {
			size_t d = g.degree(r);
			if (d <= 1) {
				// make nodes with degree 1 the root of the BFS trees
				// all other nodes have degree 2 (inner nodes) or 1 (leaves)
				nonCoreEdgesTimes2 += d;
				core[r] = false;
				return true;
			}
			return false;
		}
		bool normalEdge(size_t p, size_t c, const edge_t & edgeval) {
			(void)p;
			(void)edgeval;
			size_t d = g.degree(c);
			if (d <= 2) {
				// FIXME this idea is broken, nodes in a tree have degree 3 or more!
				// only consider nodes with degree 2 (inner nodes) or 1 (leaves)
				nonCoreEdgesTimes2 += d;
				core[c] = false;
				return true;
			}
			nonCoreEdgesTimes2 += 1;
			return false;
		}
		void cycleEdge(size_t p, size_t c, const edge_t & edgeval) {
			(void)p;
			(void)c;
			(void)edgeval;
			throw std::runtime_error("CoreFinder found cycle");
		}
	};

	class CoreAssigner {
	private:
		GraphSimple &g;
		vectorb &core;
	public:
		vector values;

		CoreAssigner(GraphSimple &g, vectorb &core)
			: g(g), core(core), values(g.getN(), 0) {
			// nothing
		}

		bool root(size_t r) {
			return core[r];
		}

		bool normalEdge(size_t p, size_t c, const edge_t & edgeval) {
			if (!core[c]) {
				return false;
			}
			// child gets parent's value XOR edge value
			values[c] = values[p] ^ edgeval;
			return true;
		}
		void cycleEdge(size_t p, size_t c, const edge_t & edgeval) {
			(void)p;
			(void)c;
			(void)edgeval;
			//throw std::runtime_error("BFS found cycle");
		}
	};

public:
	double factor_init() {
		return 1.3;
	}
	double factor_inc() {
		return 1.02;
	}


	bool run(randgen_t &randgen, const map_t &map,
			size_t maxlen, uint32_t n, size_t trials) {

		GraphSimple g(n);

		RandConst rsC0(0);
		RandConst rsC1(1);
		RandRange rs0_n(randgen, 0, n-1);
		RandRange rs1_n(randgen, 1, n-1);

		PreMult pre1(maxlen, rs1_n);
		PreMult pre2(maxlen, rs1_n);
		HashMultSum hf1(pre1, rsC0, rsC1);
		HashMultSum hf2(pre2, rs0_n, rsC1);

		vectorb core;

		bool runagain = true;
		for (size_t i=0; runagain && i<trials; i++) {
			pre1.randomize();
			pre2.randomize();
			hf1.randomize();
			hf2.randomize();
			g.clear();

			runagain = false;
			for (auto &x : map) {
				const string &key = x.first;
				uint32_t h1 = hf1.hash(key) % n;
				uint32_t h2 = hf2.hash(key) % n;
				if (h2 == h1) {
					// resolve loops

					// h2 even, n even => off = 1
					// h2 even, n odd  => off = 0
					// h2 odd,  n even => off = 0
					// h2 odd,  n odd  => off = 1
					uint32_t off = (h1 & 1) ^ (n & 1) ^ 1;

					// (2*even+1) mod even => odd
					// (2*even  ) mod odd  => odd
					// (2*odd   ) mod even => even
					// (2*odd +1) mod odd  => even
					h2 = (h1 * 2 + off) % n;
				}
				if (h1 == h2) {
					// loop detected
//					std::cout << "loop" << std::endl;
					runagain = true;
					break;
				}
				bool r = g.addEdge(h1, h2, x.second);
				if (!r) {
					//parallel detected
//					std::cout << "parallel" << std::endl;
					runagain = true;
					break;
				}
			}

			if (runagain) {
				continue;
			}

			BFS bfs;
			CoreFinder coreFinder(g, core);
			bfs.visitAll(g, coreFinder);

//			std::cout << coreFinder.nonCoreEdgesTimes2 << std::endl;
			size_t coreEdgesTimes2 = 2*map.size() - coreFinder.nonCoreEdgesTimes2;
			if (coreEdgesTimes2 > map.size()) {
//				std::cout << "core is too big" << std::endl;
				runagain = true;
				continue;
			}

			CoreAssigner coreAssigner(g, core);
			bfs.visitAll(g, coreAssigner);

			return true;
		}
		return false;
	}
};


