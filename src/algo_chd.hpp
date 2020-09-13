#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "randtools.hpp"
#include "hashtools.hpp"
#include "algo.hpp"

class AlgoCHD {
private:
	using string = std::string;
	using size_t = std::size_t;
	using vectorb = std::vector<bool>;
	using vectors = std::vector<size_t>;
	using vectorss = std::vector<vectors>;
	using vectorstr = std::vector<string>;

public:
	double factor_init() {
		return 1.02;
	}
	double factor_inc() {
		return 1.05;
	}

	bool run(randgen_t &randgen, const map_t &map,
			size_t maxlen, uint32_t n, size_t trials) {
		(void)maxlen;

		RandRange rs32Bit(randgen, 0, UINT32_MAX);

		PreNone pre1;
		PreNone pre2;
		HashJenkinsOneAtATime hf1(pre1, rs32Bit);
		HashJenkinsOneAtATime hf2(pre2, rs32Bit);


		vectorstr keys;
		for (auto &x : map) {
			keys.push_back(x.first);
		}

		vectorb taken;
		vectorss buckets;
		vectors buckethashes;

		bool runagain = true;
		for (size_t i=0; runagain && i<trials; i++) {
			pre1.randomize();
			hf1.randomize();

			uint32_t mod = 313;
			buckets.assign(mod, vectors());

			// map to buckets
			for (size_t i = 0; i<keys.size(); i++) {
				auto &key = keys[i];
				uint32_t h1 = hf1.hash(key) % mod;
				buckets[h1].push_back(i);
			}
			std::sort(buckets.begin(), buckets.end(), [](vectors &a, vectors & b) {
					return a.size() > b.size();
				});

			// reset boolean markers
			taken.assign(n, false);

			runagain = false;			
			for (auto &bucket : buckets) {
				// std::cout << "bucket" << std::endl;

				// find non-colliding hash function
				size_t trials2 = 1000;
				while (trials2 > 0) {
					pre2.randomize();
					hf2.randomize();
					bool collision = false;
					for (auto ki : bucket) {
						string &key = keys[ki];
						uint32_t h2 = hf2.hash(key) % n;
						// std::cout << "  " << key << " " << h2 << std::endl;
						if (taken[h2]) {
							collision = true;
							break;
						}
					}
					if (!collision) {
						break;
					}
					trials2--;
				}
				if (trials2 <= 0) {
					// std::cout << "failed at bucket " << bucket.size() << std::endl;
					runagain = true;
					break;
				}
				for (auto ki : bucket) {
					string &key = keys[ki];
					uint32_t h2 = hf2.hash(key) % n;
					taken[h2] = true;
				}
			}

			if (runagain) {
				continue;
			}


			return true;
		}
		return false;
	}
};


