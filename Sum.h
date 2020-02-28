#pragma once
#include "NumberSet.h"

class Sum
{
public:
    Sum(int total, int num_cells);

    int total() const;
    int num_cells() const;
    NumberSet possible() const;
    NumberSet required() const;
    void restrict_to(NumberSet possible, NumberSet required);

private:
    void update_sets();

private:
    int total_;
    int num_cells_;
    NumberSet possible_;
    NumberSet required_;
};
