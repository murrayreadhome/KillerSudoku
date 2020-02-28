#include "Sum.h"
using namespace std;

namespace
{
    const vector<NumberSet>& all_sums(int total, int num_cells)
    {
        const static auto sums = []() -> vector<vector<vector<NumberSet>>>
        {
            vector<vector<vector<NumberSet>>> sums{46, vector<vector<NumberSet>>{10}};
            vector<NumberSet> known{1,NumberSet::none()};
            vector<NumberSet> next;
            for (int i=1; i<=9; i++)
            {
                next.clear();
                for (NumberSet s : known)
                {
                    NumberSet n = s.add(i);
                    sums[n.sum()][n.size()].push_back(n);
                    next.push_back(n);
                }
                known.swap(next);
            }
            return sums;
        }();
        return sums[total][num_cells];
    }
}

Sum::Sum(int total, int num_cells)
    : total_(total), 
    num_cells_(num_cells), 
    possible_(NumberSet::all()),
    required_(NumberSet::none())
{
    update_sets();
}

int Sum::total() const
{
    return total_;
}

int Sum::num_cells() const
{
    return num_cells_;
}

NumberSet Sum::possible() const
{
    return possible_;
}

NumberSet Sum::required() const
{
    return required_;
}

void Sum::restrict_to(NumberSet possible, NumberSet required)
{
    NumberSet pwas = possible_;
    possible_ = possible_.intersection(possible);
    NumberSet rwas = required_;
    required_ = required_.add(required);
    if (possible_ != pwas || required_ != rwas)
        update_sets();
}

void Sum::update_sets()
{
    const vector<NumberSet>& sets = all_sums(total_, num_cells_);
    NumberSet calc_possible(NumberSet::none());
    NumberSet calc_required(NumberSet::all());
    for (NumberSet sum : sets)
    {
        if (possible_.intersection(sum) != sum)
            continue;
        if (required_.intersection(sum) != required_)
            continue;
        calc_possible = calc_possible.add(sum);
        calc_required = calc_required.intersection(sum);
    }
    possible_ = calc_possible;
    required_ = calc_required;
}

