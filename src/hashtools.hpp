#pragma once

#include <string>
#include <memory>

#include "randtools.hpp"

class Preprocessor {
public:
	virtual ~Preprocessor() {
		// nothing
	}

	virtual uint32_t preprocess(size_t i, char c) = 0;
	virtual void randomize() = 0;
};

class HashFunc {
public:
	virtual ~HashFunc() {
		// nothing
	}

	virtual uint32_t hash(const std::string &s) = 0;
	virtual void randomize() = 0;
};



class PreNone : public Preprocessor {
public:
	PreNone() {
		// nothing
	}

	uint32_t preprocess(size_t i, char c) override {
		return (unsigned char)c;
	}

	void randomize() override {
		// nothing
	}
};

class PreMult : public Preprocessor {
private:
	const size_t n;
	RandSource &rs;
	std::unique_ptr<uint32_t[]> table;

public:
	PreMult(size_t n, RandSource &rs) : n(n), rs(rs) {
		table = std::make_unique<uint32_t[]>(n);
		for (size_t i=0; i<n; i++) {
			table[i] = 1;
		}
	}

	uint32_t preprocess(size_t i, char c) override {
		if (i >= n) {
			throw std::runtime_error("internal error");
		}
		return table[i] * (unsigned char)c;
	}

	void randomize() override {
		for (size_t i=0; i<n; i++) {
			table[i] = rs.get();
		}
	}
};

class PreXOR : public Preprocessor {
private:
	const size_t n;
	RandSource &rs;
	std::unique_ptr<uint32_t[]> table;

public:
	PreXOR(size_t n, RandSource &rs) : n(n), rs(rs) {
		table = std::make_unique<uint32_t[]>(n);
		for (size_t i=0; i<n; i++) {
			table[i] = 0;
		}
	}

	uint32_t preprocess(size_t i, char c) override {
		if (i >= n) {
			throw std::runtime_error("internal error");
		}
		return table[i] ^ (unsigned char)c;
	}

	void randomize() override {
		for (size_t i=0; i<n; i++) {
			table[i] = rs.get();
		}
	}
};


class HashMultSum : public HashFunc {
private:
	Preprocessor &p;
	RandSource &rseed;
	RandSource &rfactor;
	uint32_t factor = 1;
	uint32_t seed = 0;

public:
	HashMultSum(Preprocessor &p, RandSource &rseed,
			RandSource &rfactor) : p(p), rseed(rseed), rfactor(rfactor) {
		// nothing
	}

	uint32_t hash(const std::string &s) {
		uint32_t r = seed;
		size_t len = s.length();
		for (size_t i=0; i<len; i++) {
			r = r * factor + p.preprocess(i, s[i]);
		}
		return r;
	}

	void randomize() override {
		seed = rseed.get();
		factor = rfactor.get();
	}
};


class HashJenkinsOneAtATime : public HashFunc {
private:
	// https://en.wikipedia.org/wiki/Jenkins_hash_function
	Preprocessor &p;
	RandSource &rseed;
	uint32_t seed = 0;

public:
	HashJenkinsOneAtATime(Preprocessor &p, RandSource &rseed) : p(p), rseed(rseed) {
		// nothing
	}

	uint32_t hash(const std::string &s) {
		size_t len = s.length();
		uint32_t hash = seed;
		for (size_t i=0; i<len; i++) {
			hash += p.preprocess(i, s[i]);
			hash += hash << 10;
			hash ^= hash >> 6;
		}
		// also hash length ?
		// hash += len;
		hash += hash << 3;
		hash ^= hash >> 11;
		hash += hash << 15;
		return hash;
	}

	void randomize() override {
		seed = rseed.get();
	}
};
