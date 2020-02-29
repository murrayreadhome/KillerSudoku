#pragma once
#include "NumberSet.h"
#include "IdSet.h"

class SumSet
{
public:
    SumSet(int total, int num_cells);

    int total() const;
    int num_cells() const;
    NumberSet possible() const;
    NumberSet required() const;
    void restrict_possible(NumberSet possible);
    void add_required(NumberSet required);
    void remove_cell(int value);

    size_t id;
    IdSet cells;
    char name='.';
    
private:
    void update_sets();

private:
    int total_;
    int num_cells_;
    NumberSet possible_;
    NumberSet required_;
};
