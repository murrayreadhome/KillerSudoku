#pragma once
#include "NumberSet.h"
#include "IdSet.h"

const int Unknown = 0;

class Cell
{
public:
    Cell();

    int value() const;
    void set_value(int value);

    NumberSet numbers() const;
    void set_numbers(NumberSet numbers);

    size_t id;
    IdSet sums;
    IdSet lessthans;
    
private:
    int value_;
    NumberSet numbers_;
};
