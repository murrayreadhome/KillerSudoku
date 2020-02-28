#pragma once
#include "NumberSet.h"

const int Unknown = 0;

class Cell
{
public:
    Cell();

    int value() const;
    void set_value(int value);

    NumberSet numbers() const;
    void set_numbers(NumberSet numbers);

private:
    int value_;
    NumberSet numbers_;
};
