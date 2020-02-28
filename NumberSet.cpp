#include "NumberSet.h"
#include <numeric>
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
    return {1<<(n-1)};    
}

NumberSet NumberSet::all()
{
    return {(1<<9)-1};
}

NumberSet NumberSet::none()
{
    return {0};
}

NumberSet NumberSet::intersection(const NumberSet& s) const
{
    return {bits_ & s.bits_};
}

NumberSet NumberSet::add(int n) const
{
    return add(single(n));
}

NumberSet NumberSet::add(const NumberSet& s) const
{
    return {bits_ | s.bits_};
}

NumberSet NumberSet::remove(const NumberSet& s) const
{
    return {bits_ & ~s.bits_};
}

NumberSet NumberSet::invert() const
{
    return {((1<<9)-1) & ~bits_};
}
