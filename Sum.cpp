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
            vector<NumberSet> next = known;
            for (int i=1; i<=9; i++)
            {
                for (NumberSet s : known)
                {
                    NumberSet n = s.add(i);
                    sums[n.sum()][n.size()].push_back(n);
                    next.push_back(n);
                }
                known = next;
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

void Sum::restrict_possible(NumberSet possible)
{
    NumberSet pwas = possible_;
    possible_ = possible_.intersection(possible);
    if (possible_ != pwas)
        update_sets();
}

void Sum::add_required(NumberSet required)
{
    NumberSet rwas = required_;
    required_ = required_.add(required);
    if (required_ != rwas)
        update_sets();
}

void Sum::remove_cell(int value)
{
    total_ -= value;
    num_cells_--;
    restrict_possible(NumberSet::single(value).invert());
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

#include "gtest/gtest.h"

TEST(Sum, Ctor)
{
    EXPECT_EQ(1,Sum(2,1).num_cells());
    EXPECT_EQ(2,Sum(2,1).total());
    EXPECT_EQ(NumberSet::single(2),Sum(2,1).possible());
    EXPECT_EQ(NumberSet::single(2),Sum(2,1).required());
}

TEST(Sum, Possible)
{
    EXPECT_EQ(NumberSet({1,2,3,4}),Sum(5,2).possible());
    EXPECT_EQ(NumberSet({1,3}),Sum(4,2).possible());
}

TEST(Sum, Required)
{
    EXPECT_EQ(NumberSet::none(),Sum(5,2).required());
    EXPECT_EQ(NumberSet({1,3}),Sum(4,2).required());
    EXPECT_EQ(NumberSet::single(1),Sum(13,4).required());
}

TEST(Sum, Restrict)
{
    Sum s(5,2);
    s.restrict_possible(NumberSet({2,3,4}));
    EXPECT_EQ(NumberSet({2,3}),s.possible());
    EXPECT_EQ(NumberSet({2,3}),s.required());
}

TEST(Sum, Add)
{
    Sum s(5,2);
    s.add_required(NumberSet::single(1));
    EXPECT_EQ(NumberSet({1,4}),s.possible());
    EXPECT_EQ(NumberSet({1,4}),s.required());
}

TEST(Sum, Remove)
{
    Sum s(5,2);
    s.remove_cell(2);
    EXPECT_EQ(3, s.total());
    EXPECT_EQ(1, s.num_cells());
    EXPECT_EQ(NumberSet::single(3),s.possible());
    EXPECT_EQ(NumberSet::single(3),s.required());
}
