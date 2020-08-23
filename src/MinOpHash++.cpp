#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <climits>

#include <json/json.h>

#include "graph.hpp"
#include "bfs.hpp"
#include "unionfind.hpp"
#include "randtools.hpp"
#include "primetest.hpp"
#include "hashtools.hpp"
#include "algo_chm.hpp"
#include "algo_bmz.hpp"

using std::size_t;
using std::uint32_t;
using std::uint64_t;
using std::deque;
using std::vector;
using std::string;
using std::seed_seq;
using std::unordered_map;
using std::uniform_int_distribution;



uint32_t fastmod(uint32_t x, uint32_t mod) {
	if (mod <= 1) {
		return 0;
	}

	int l = sizeof(unsigned long) * CHAR_BIT - __builtin_clzl(mod);
	if (l < 32) {
		// loop runs at most 31 times (e.g. 0xFFFF_FFFF mod 2)
		while (1) {
			uint32_t y = x >> l;
			if (y == 0) {
				break;
			}
			x -= y * mod;
		}
	}
	if (x >= mod) {
		x -= mod;
	}
	return x;
}


typedef uint64_t edge_t;




template<class R>
void seedMT(R &randgen) {
	size_t seed_count = R::state_size * (R::word_size + 31)/32;
	vector<uint32_t> seeds(seed_count);
	uniform_int_distribution<uint32_t> d;
	std::random_device rd;
	for (size_t i = 0; i < seed_count; i++) {
		seeds[i] = d(rd);
	}
	seed_seq seq(seeds.cbegin(), seeds.cend());
	randgen.seed(seq);
}


void testFastMod() {
	for (uint32_t i=0; i<21; i++) {
		uint32_t r = fastmod(i,7);
		std::cout << i << " mod 7 = " << r << std::endl;
	}
	for (uint32_t i=0; i<24; i++) {
		uint32_t r = fastmod(i,8);
		std::cout << i << " mod 8 = " << r << std::endl;
	}
	uint32_t r = fastmod(0xFFFFFFFF,16);
	std::cout << 0xFFFFFFFF << " mod 16 = " << r << std::endl;
	r = fastmod(0xFFFFFFFF,8);
	std::cout << 0xFFFFFFFF << " mod 8 = " << r << std::endl;
	r = fastmod(0xFFFFFFFF,4);
	std::cout << 0xFFFFFFFF << " mod 4 = " << r << std::endl;
	r = fastmod(0xFFFFFFFF,2);
	std::cout << 0xFFFFFFFF << " mod 2 = " << r << std::endl;
	std::cout << 15 << " mod 0x10001000 = " << fastmod(15,0x10001000) << std::endl;
}

template <class R>
void testIsPrime(R &randgen) {
	std::cout << isPrime(1961, 100, randgen) << std::endl;
	std::cout << isPrime(167077, 100, randgen) << std::endl;

	for (uint32_t i=1; i<1000000; i++) {
		if (isPrime(i, 100, randgen)) {
			std::cout << i << std::endl;
		}
	}
}

void loadJSON(map_t &map, string filename) {
	std::ifstream input(filename);

	Json::Value root;
	input >> root;

	map.clear();
	if (root.isArray()) {
		// array
		edge_t c = 0;
		for (Json::Value &x : root) {
			if (!x.isString()) {
				throw std::runtime_error("non-string in array");
			}
			string key = x.asString();
			auto r = map.insert(map_t::value_type(key, c++));
			if (!r.second) {
				throw std::runtime_error("duplicate in array");
			}
		}
	} else if (root.isObject()) {
		// dictionary
		for (auto it = root.begin(); it != root.end(); ++it) {
			// keys should be strings
			string key = it.key().asString();
			Json::Value &val = *it;
			if (!val.isUInt64()) {
				throw std::runtime_error("non-uin64 value in dictionary");
			}
			auto r = map.insert(map_t::value_type(key, val.asUInt64()));
			if (!r.second) {
				throw std::runtime_error("duplicate key in dictionary");
			}
		}
	} else {
		throw std::runtime_error("only dictionaries and arrays are supported");
	}
}


std::pair<size_t, size_t> minMax(const map_t &map) {
	size_t minlen, maxlen;
	if (map.empty()) {
		minlen = 0;
		maxlen = 0;
	} else {
		auto s = map.begin();
		auto e = map.end();
		maxlen = minlen = s->first.length();
		while (++s != e) {
			size_t x = s->first.length();
			if (x > maxlen) {
				maxlen = x;
			}
			if (x < minlen) {
				minlen = x;
			}
		}
	}
	return std::make_pair(minlen, maxlen);
}


int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	randgen_t randgen;
	seedMT(randgen);

//	testFastMod();
//	testIsPrime(randgen);

	unordered_map<string, edge_t> map;
	loadJSON(map, "tests/words-google-10000-english.json");
//	loadJSON(map, "tests/words-linux.json");
//	loadJSON(map, "tests/words-utf16.json");
//	loadJSON(map, "tests/words-small.json");
//	loadJSON(map, "tests/words-small0.json");
//	loadJSON(map, "tests/words-small1.json");
//	loadJSON(map, "tests/words-small2.json");
//	loadJSON(map, "tests/words-small3.json");

	//TODO zero characters in strings should not be allowed

	if (map.size() > UINT32_MAX) {
		throw std::runtime_error("too many words");
	}

	size_t minlen, maxlen;
	std::tie(minlen, maxlen) = minMax(map);

	std::cout << "minlen = " << minlen << " and maxlen = " << maxlen << std::endl;

	size_t m = map.size();
	uint64_t min = m;
	if (min < 2) {
		min = 2;
	}

	size_t trials = 100;

//	AlgoCHM algo;
	AlgoBMZ algo;

	double fi = algo.factor_init();
	double f = algo.factor_inc();
	for (double n = (double)m * fi; ; n *= f) {
		uint64_t ni64 = (uint64_t)(n + 0.5);
		if (ni64 > UINT32_MAX) {
			throw std::runtime_error("too many elements");
		}

		uint32_t ni32 = (uint32_t)ni64;
		while (!PrimeTest::isPrime(ni32, PRIMETEST_DEFAULT_ROUNDS, randgen)) {
			ni32++;
		}
		if (ni32 < min) {
			continue;
		}
		min = (uint64_t)ni32 + 1;

		std::cout << "m = " << m << " and n = " << ni32
				<< " (factor " << ni32/(double)m << ")" << std::endl;

		if (algo.run(randgen, map, maxlen, ni32, trials)) {
			break;
		}
	}
	return 0;
}



