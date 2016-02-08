#ifndef STATISTIC_H
#define STATISTIC_H

#include <vector>

class Statistic
{
public:
    Statistic();
    virtual void compute() = 0;
    virtual void histogram(std::vector &x, std::vector &y);
};

#endif // STATISTIC_H
