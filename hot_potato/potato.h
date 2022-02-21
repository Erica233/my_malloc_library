#ifndef __POTATO_H__
#define __POTATO_H__

#include <vector>

#define MAX_HOPS 512
#define MAX_HOPS_IDX 511

class Potato {
public:
    int num_hops;
    //std::vector<int> ids;
    int curr_rnd;
    int ids[MAX_HOPS] = {};

    Potato(): num_hops(0), curr_rnd(0) {}
    explicit Potato(int n): num_hops(n), curr_rnd(0) {}
};

#endif
