#include "Board.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <map>
#include <sstream>
using namespace std;


Board::Board()
: cells_(81), sums_(27, SumSet(45, 9))
{
    names_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    reverse(names_.begin(), names_.end());

    for (size_t i=0; i<cells_.size(); i++)
        cells_[i].id = i;
    for (size_t i=0; i<sums_.size(); i++)
        sums_[i].id = i;
    
    // rows
    for (size_t y=0; y<9; y++)
    {
        for (size_t x=0; x<9; x++)
        {
            size_t cell = y*9 + x;
            link(y, cell);
        }
    }
    
    // columns
    for (size_t y=0; y<9; y++)
    {
        for (size_t x=0; x<9; x++)
        {
            size_t col = x+9;
            size_t cell = y*9 + x;
            link(col, cell);
        }
    }

    // blocks
    for (size_t b=0; b<9; b++)
    {
        for (size_t s=0; s<9; s++)
        {
            size_t block = b+18;
            size_t cell = (b/3)*27 + (s/3)*9 + (b%3)*3 + (s%3);
            link(block, cell);
        }
    }
}

void Board::add_sum(const IdSet& cells, int total)
{
    SumSet s(total, cells.size());
    s.cells = cells;
    s.id = sums_.size();
    s.name = names_.back();
    names_.pop_back();
    sums_.push_back(s);
    for (size_t cell : cells)
        cells_[cell].sums.add(s.id);
}

void Board::set_cell(size_t cell, int value)
{
    Cell& c = cells_[cell];
    c.set_value(value);
    for (size_t s : c.sums)
    {
        sums_[s].cells.remove(cell);
        sums_[s].remove_cell(value);
    }
}

void Board::link(size_t sum, size_t cell)
{
    sums_[sum].cells.add(cell);
    cells_[cell].sums.add(sum);
}

std::ostream& operator<<(std::ostream& out, const Board& b)
{
    set<size_t> sums_used;
    for (size_t y=0; y<9; y++)
    {
        for (size_t x=0; x<9; x++)
        {
            const Cell& cell = b.cells_[y*9 + x];
            if (cell.value() != Unknown)
                out << cell.value();
            else
            {
                size_t sum_id = *max_element(cell.sums.begin(), cell.sums.end());
                if (sum_id >= 27)
                    sums_used.insert(sum_id);
                out << b.sums_[sum_id].name;
            }
        }
        out << endl;
    }
    out << sums_used.size() << endl;
    for (size_t sum_id : sums_used)
    {
        const SumSet& sum = b.sums_[sum_id];
        out << sum.name << " " << sum.total() << endl;
    }
}

std::istream& operator>>(std::istream& in, Board& b)
{
    map<char, IdSet> sum_cells;
    for (size_t y=0; y<9; y++)
    {
        for (size_t x=0; x<9; x++)
        {
            size_t cell = y*9 + x;
            char c;
            in >> c;
            if ('1'<=c && c<='9')
                b.set_cell(cell, c-'0');
            else
                sum_cells[c].add(cell);
        }
        in.get();
    }
    size_t n = 0;
    in >> n; in.get();
    for (size_t i=0; i<n; i++)
    {
        char c;
        int t;
        in >> c >> t;
        in.get();
        b.add_sum(sum_cells[c], t);
    }
}

#include "gtest/gtest.h"

TEST(Board, Sudoku)
{
    string sudoku = R"(53..7....
6..195...
.98....6.
8...6...3
4..8.3..1
7...2...6
.6....28.
...419..5
....8..79)";
    stringstream in(sudoku);
    Board b;
    in >> b;
    cout << b;
}
