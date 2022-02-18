#ifndef __POTATO_H__
#define __POTATO_H__
#include <vector>
#define MAX_HOPS 512

class Potato {
public:
    int num_hops;
    //std::vector<int> ids;
    int ids[MAX_HOPS] = {-1};

    Potato(): num_hops(0) {}
    explicit Potato(int n): num_hops(n) {}
};

#endif
