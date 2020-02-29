#include "SumSet.h"
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

SumSet::SumSet(int total, int num_cells)
    : total_(total), 
    num_cells_(num_cells), 
    possible_(NumberSet::all()),
    required_(NumberSet::none())
{
    update_sets();
}

int SumSet::total() const
{
    return total_;
}

int SumSet::num_cells() const
{
    return num_cells_;
}

NumberSet SumSet::possible() const
{
    return possible_;
}

NumberSet SumSet::required() const
{
    return required_;
}

void SumSet::restrict_possible(NumberSet possible)
{
    NumberSet pwas = possible_;
    possible_ = possible_.intersection(possible);
    if (possible_ != pwas)
        update_sets();
}

void SumSet::add_required(NumberSet required)
{
    NumberSet rwas = required_;
    required_ = required_.add(required);
    if (required_ != rwas)
        update_sets();
}

void SumSet::remove_cell(int value)
{
    total_ -= value;
    num_cells_--;
    restrict_possible(NumberSet::single(value).invert());
}


void SumSet::update_sets()
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

TEST(SumSet, Ctor)
{
    EXPECT_EQ(1,SumSet(2,1).num_cells());
    EXPECT_EQ(2,SumSet(2,1).total());
    EXPECT_EQ(NumberSet::single(2),SumSet(2,1).possible());
    EXPECT_EQ(NumberSet::single(2),SumSet(2,1).required());
}

TEST(SumSet, Possible)
{
    EXPECT_EQ(NumberSet({1,2,3,4}),SumSet(5,2).possible());
    EXPECT_EQ(NumberSet({1,3}),SumSet(4,2).possible());
}

TEST(SumSet, Required)
{
    EXPECT_EQ(NumberSet::none(),SumSet(5,2).required());
    EXPECT_EQ(NumberSet({1,3}),SumSet(4,2).required());
    EXPECT_EQ(NumberSet::single(1),SumSet(13,4).required());
}

TEST(SumSet, Restrict)
{
    SumSet s(5,2);
    s.restrict_possible(NumberSet({2,3,4}));
    EXPECT_EQ(NumberSet({2,3}),s.possible());
    EXPECT_EQ(NumberSet({2,3}),s.required());
}

TEST(SumSet, Add)
{
    SumSet s(5,2);
    s.add_required(NumberSet::single(1));
    EXPECT_EQ(NumberSet({1,4}),s.possible());
    EXPECT_EQ(NumberSet({1,4}),s.required());
}

TEST(SumSet, Remove)
{
    SumSet s(5,2);
    s.remove_cell(2);
    EXPECT_EQ(3, s.total());
    EXPECT_EQ(1, s.num_cells());
    EXPECT_EQ(NumberSet::single(3),s.possible());
    EXPECT_EQ(NumberSet::single(3),s.required());
}
