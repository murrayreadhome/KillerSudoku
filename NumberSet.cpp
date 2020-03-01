#include "NumberSet.h"
#include <numeric>
#include "gtest/gtest.h"
using namespace std;

NumberSet::NumberSet()
: bits_(0)
{}
NumberSet::NumberSet(int bits)
: bits_(bits)
{}
NumberSet::NumberSet(const vector<int>& nums)
: bits_(0)
{
    for (int i : nums)
        bits_ |= 1<<(i-1);
}

bool NumberSet::operator==(const NumberSet& s) const
{
    return bits_ == s.bits_;
}

bool NumberSet::operator!=(const NumberSet& s) const
{
    return bits_ != s.bits_;
}

int NumberSet::bits() const
{
    return bits_;
}

const vector<int>& NumberSet::nums() const
{
    const static auto expanded = []() -> vector<vector<int>>
    {
        vector<vector<int>> expanded(1<<9);
        for (int i=0; i<(1<<9); i++)
        {
            for (int b=0; b<9; b++)
            {
                if (i & (1<<b))
                    expanded[i].push_back(b+1);
            }
        }
        return expanded;
    }();
    return expanded[bits_];
}

size_t NumberSet::size() const
{
    const static auto sizes = []() -> vector<size_t>
    {
        vector<size_t> sizes(1<<9);
        for (int i=0; i<(1<<9); i++)
            sizes[i] = NumberSet(i).nums().size();
        return sizes;
    }();
    return sizes[bits_];
}

bool NumberSet::empty() const
{
    return bits_ == 0;
}

int NumberSet::sum() const
{
    const static auto sums = []() -> vector<int>
    {
        vector<int> sums(1<<9);
        for (int i=0; i<(1<<9); i++)
        {
            const auto& nums = NumberSet(i).nums();
            sums[i] = accumulate(begin(nums), end(nums), 0);
        }
        return sums;
    }();
    return sums[bits_];
}

NumberSet NumberSet::single(int n)
{
    return NumberSet{1<<(n-1)};    
}

NumberSet NumberSet::all()
{
    return NumberSet{(1<<9)-1};
}

NumberSet NumberSet::none()
{
    return NumberSet{0};
}

NumberSet NumberSet::intersection(const NumberSet& s) const
{
    return NumberSet{bits_ & s.bits_};
}

bool NumberSet::contains(int n) const
{
    return bits_ & 1<<(n-1);
}

NumberSet NumberSet::add(int n) const
{
    return add(single(n));
}

NumberSet NumberSet::add(const NumberSet& s) const
{
    return NumberSet{bits_ | s.bits_};
}

NumberSet NumberSet::remove(const NumberSet& s) const
{
    return NumberSet{bits_ & ~s.bits_};
}

NumberSet NumberSet::invert() const
{
    return NumberSet{((1<<9)-1) & ~bits_};
}

TEST(NumberSet, Ctors)
{
    EXPECT_EQ(0, NumberSet().bits());
    EXPECT_EQ(3, NumberSet({1,2}).bits());
    EXPECT_EQ(NumberSet({3,4}), NumberSet({4,3}));
    EXPECT_EQ(NumberSet(), NumberSet::none());
    EXPECT_EQ(NumberSet({1,2,3,4,5,6,7,8,9}), NumberSet::all());
    EXPECT_EQ(NumberSet(vector<int>{5}), NumberSet::single(5));
}

TEST(NumberSet, Access)
{
    EXPECT_EQ(vector<int>({4,8}), NumberSet({4,8}).nums());
    ASSERT_TRUE(NumberSet().empty());
    EXPECT_EQ(3u, NumberSet({2,4,7}).size());
    EXPECT_EQ(12, NumberSet({1,3,8}).sum());
}

TEST(NumberSet, Ops)
{
    EXPECT_EQ(NumberSet({3,4}), NumberSet({2,3,4}).intersection(NumberSet({3,4,5})));
    EXPECT_EQ(NumberSet({2,3,4}), NumberSet({2,3}).add(NumberSet({2,4})));
    EXPECT_EQ(NumberSet({2,3,4}), NumberSet({2,3}).add(4));
    EXPECT_EQ(NumberSet({2,4}), NumberSet({2,3,4}).remove(NumberSet::single(3)));    
    EXPECT_EQ(NumberSet({1,2,3,4,5}), NumberSet({6,7,8,9}).invert());
}
