#ifndef __POTATO_H__
#define __POTATO_H__
#include <vector>

class Potato {
private:
    int num_hops;
    std::vector<std::string> ids;
public:
    explicit Potato(int n): num_hops(n) {}

};

#endif
