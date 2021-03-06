#include <sys/time.h>
#include <tsl/hopscotch_map.h>
#include <tsl/robin_map.h>
#include <boost/unordered/unordered_map.hpp>
#include <iomanip>
#include <iostream>
#include <map>
#include <sparsehash/dense_hash_map>
#include <unordered_map>

int nowUs() {
    timeval now = {0, 0};
    gettimeofday(&now, NULL);

    return int(now.tv_sec * 1000000 + now.tv_usec);
}

struct nohashint {
    size_t operator()(const int i) const {
        return i;
    }
};

const int N = 3000000;
int i;

#define MapFind()                     \
    decltype(m)::iterator it;         \
    for (int i = 0; i < N; i++) {     \
        m[i] = i;                     \
    }                                 \
    int t1 = nowUs();                 \
    for (int l = 0; l < 100; l++) {   \
        for (int i = 0; i < N; i++) { \
            it = m.find(i);           \
        }                             \
    }                                 \
    int t2 = nowUs();                 \
    i = it->second;  // prevent -O2

#define MapFindSparseHash() \
    m.set_empty_key(-1);    \
    MapFind()

#define Elapse(key)                                                \
    std::cout << std::setw(50) << std::setiosflags(std::ios::left) \
              << key                                               \
              << " => " << t2 - t1 << std::endl;

#define BenchmarkMap(namespace, class, process)          \
    void benchmark_##namespace##_##class() {             \
        {                                                \
            namespace ::class<int, int> m;               \
            process();                                   \
            Elapse(#namespace "::" #class "<int, int>"); \
        }                                                \
    }

#define BenchmarkUnorderedMap(namespace, class, process)            \
    void benchmark_##namespace##_##class() {                        \
        {                                                           \
            namespace ::class<int, int> m;                          \
            process();                                              \
            Elapse(#namespace "::" #class "<int, int>");            \
        }                                                           \
        {                                                           \
            namespace ::class<int, int, nohashint> m;               \
            process();                                              \
            Elapse(#namespace "::" #class "<int, int, nohashint>"); \
        }                                                           \
        {                                                           \
            namespace ::class<int, int> m(N);                       \
            process();                                              \
            Elapse(#namespace "::" #class "<int, int>(N)");         \
        }                                                           \
    }

BenchmarkMap(std, map, MapFind);
BenchmarkUnorderedMap(std, unordered_map, MapFind);
BenchmarkUnorderedMap(boost, unordered_map, MapFind);
BenchmarkUnorderedMap(google, dense_hash_map, MapFindSparseHash);
BenchmarkUnorderedMap(tsl, hopscotch_map, MapFind);
BenchmarkUnorderedMap(tsl, robin_map, MapFind);

int main(int argc, char *argv[]) {
    benchmark_std_map();
    benchmark_std_unordered_map();
    benchmark_boost_unordered_map();
    benchmark_google_dense_hash_map();
    benchmark_tsl_hopscotch_map();
    benchmark_tsl_robin_map();
}
