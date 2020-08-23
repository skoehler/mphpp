#pragma once

#include <random>

#define PRIMETEST_DEFAULT_ROUNDS 16

class PrimeTest {
private:
	using uint32_t = std::uint32_t;

	static inline uint32_t mulmod(uint32_t a, uint32_t b, uint32_t mod) {
		uint64_t r = (uint64_t)a * b;
		return (uint32_t)(r % mod);
	}

	static inline uint32_t powmod(uint32_t b, uint32_t e, uint32_t mod) {
		uint32_t r = 1;
		while (e > 0) {
			if (e % 2 != 0) {
				r = mulmod(r, b, mod);
			}
			b = mulmod(b, b, mod);
			e /= 2;
		}
		return r;
	}

	static bool millerRabinTest(uint32_t a, uint32_t d, uint32_t n) {
		uint32_t x = powmod(a, d, n);
		if (x == 1 || x == n-1) {
			return true;
		}
		while (d < n/2) {
			x = mulmod(x, x, n);
			if (x == 1) {
				// no need to continue
				break;
			}
			if (x == n-1) {
				return true;
			}
			d *= 2;
		}
		return false;
	}

public:
	template <class R>
	static bool isPrime(uint32_t n, uint32_t k, R &randgen) {
		if (n <= 3) {
			// handle 0, 1, 2, and 3
			return n >= 2;
		}
		if (n % 2 == 0) {
			// handle even numbers (2 is handled above)
			return false;
		}

		uint32_t d = n - 1;
		d >>= __builtin_ctzl(d);

		std::uniform_int_distribution<uint32_t> da(2, n - 2);
		for (size_t i = 0; i < k; i++) {
			uint32_t a = da(randgen);
			if (!millerRabinTest(a, d, n)) {
				return false;
			}
		}
		return true;
	}

};

