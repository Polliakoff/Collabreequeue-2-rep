#pragma once
#include <cstdlib>

class pattern
{
public:
    double swing;
    double speed;
    double h_factor;
    pattern();
    pattern(pattern &a, pattern &b);
    ~pattern() = default;
};
