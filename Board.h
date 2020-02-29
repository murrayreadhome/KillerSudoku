#pragma once
#include "Cell.h"
#include "SumSet.h"
#include <iosfwd>
#include <string>

class Board
{
public:
    Board();

    void add_sum(const IdSet& cells, int total);
    void set_cell(size_t cell, int value);

private:
    void link(size_t sum, size_t cell);

    friend std::ostream& operator<<(std::ostream& out, const Board& b);
    friend std::istream& operator>>(std::istream& in, Board& b);

private:
    std::vector<Cell> cells_;
    std::vector<SumSet> sums_;
    std::string names_;
};

std::ostream& operator<<(std::ostream& out, const Board& b);
std::istream& operator>>(std::istream& in, Board& b);
