#ifndef PARALLEL_SORT_HEADER
#define PARALLEL_SORT_HEADER

#include <algorithm>
#include <iterator>

template<class RandomIt>
void parallel_sort(const RandomIt first, const RandomIt last)
{
    constexpr typename std::iterator_traits<RandomIt>::difference_type thresh = 256;
    if(std::distance(first, last) <= thresh) {
        std::sort(first, last);
        return;
    }
    #pragma omp parallel for
    for(RandomIt i = first; i < last; i += thresh) {
        std::sort(i, std::min(i+thresh, last));
    }
    for(auto len = thresh; len < std::distance(first, last); len *= 2) {
        #pragma omp parallel for
        for(RandomIt i = first; i < last; i += 2*len) {
            std::inplace_merge(i, std::min(i+len, last), std::min(i+2*len, last));
        }
    }
}

template<class RandomIt, class Compare>
void parallel_sort(RandomIt first, RandomIt last, const Compare& comp)
{
    constexpr typename std::iterator_traits<RandomIt>::difference_type thresh = 256;
    if(std::distance(first, last) <= thresh) {
        std::sort(first, last, comp);
        return;
    }
    #pragma omp parallel for
    for(RandomIt i = first; i < last; i += thresh) {
        std::sort(i, std::min(i+thresh, last), comp);
    }
    for(auto len = thresh; len < std::distance(first, last); len *= 2) {
        #pragma omp parallel for
        for(RandomIt i = first; i < last; i += 2*len) {
            std::inplace_merge(i, std::min(i+len, last), std::min(i+2*len, last), comp);
        }
    }
}

#endif//PARALLEL_SORT_HEADER
