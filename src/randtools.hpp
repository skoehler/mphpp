#pragma once

#include <random>
#include <vector>

#include "primetest.hpp"


using randgen_t = std::mt19937_64;
using randval_t = std::uint32_t;

class RandSource {
public:
	virtual ~RandSource() {
		// nothing
	}
	virtual randval_t get() = 0;
};

class RandConst : public RandSource {
private:
	const randval_t x;
public:
	RandConst(randval_t x) : x(x) {
		// nothing
	}
	randval_t get() override {
		return x;
	}
};


class RandSource2 : public RandSource {
protected:
	randgen_t &randgen;
public:
	RandSource2(randgen_t &randgen) : randgen(randgen) {
		// nothing
	}
};

class RandRange : public RandSource2 {
private:
	std::uniform_int_distribution<randval_t> d;
public:
	RandRange(randgen_t &randgen, randval_t start, randval_t end)
		: RandSource2(randgen), d(start, end) {
		// nothing
	}
	randval_t get() override {
		return d(this->randgen);
	}
};

class RandPrime : public RandSource2 {
private:
	std::uniform_int_distribution<randval_t> d;
public:
	RandPrime(randgen_t &randgen, randval_t start, randval_t end)
		: RandSource2(randgen), d(start, end) {
		// nothing
	}
	randval_t get() override {
		while (1) {
			randval_t x = d(this->randgen);
			if (PrimeTest::isPrime(x, PRIMETEST_DEFAULT_ROUNDS, this->randgen)) {
				return x;
			}
		}
	}
};

class RandList : public RandSource2 {
private:
	std::vector<randval_t> list;
	std::uniform_int_distribution<size_t> d;
public:
	RandList(randgen_t &randgen, const std::vector<randval_t> &list)
		: RandSource2(randgen), list(list) {
		size_t s = list.size();
		if (s <= 0) {
			throw std::runtime_error("internal error");
		}
		d = std::uniform_int_distribution<size_t>(0, s - 1);
	}

	randval_t get() override {
		return list[d(this->randgen)];
	}
};

