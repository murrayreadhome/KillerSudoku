#include "Cell.h"
#include <stdexcept>
using namespace std;

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
    size_t n = numbers_.size();
    if (n == 1)
        value_ = numbers_.nums().front();
    else if (n == 0)
        throw runtime_error("dead cell");
    else
        value_ = Unknown;
}

#include "gtest/gtest.h"

TEST(Cell, Cell)
{
    Cell c;
    EXPECT_EQ(Unknown, c.value());
    c.set_value(2);
    EXPECT_EQ(NumberSet::single(2), c.numbers());
    c.set_numbers(NumberSet({3,5}));
    EXPECT_EQ(Unknown, c.value());
    c.set_numbers(NumberSet::single(5));
    EXPECT_EQ(5, c.value());
}
