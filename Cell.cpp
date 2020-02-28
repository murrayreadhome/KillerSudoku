#include "Cell.h"

Cell::Cell()
: value_(Unknown), numbers_(NumberSet::all())
{
}

int Cell::value() const
{
    return value_;
}

void Cell::set_value(int value)
{
    value_ = value;
    if (value != Unknown)
        numbers_ = NumberSet::single(value);
}

NumberSet Cell::numbers() const
{
    return numbers_;
}

void Cell::set_numbers(NumberSet numbers)
{
    numbers_ = numbers;
    if (numbers_.size() == 1)
        value_ = numbers_.nums().front();
}
