#pragma once
#include <vector>
#include <stddef.h>

class NumberSet
{
public:
    NumberSet();
    NumberSet(int bits);
    NumberSet(const std::vector<int>& nums);

    int bits() const;
    const std::vector<int>& nums() const;
    size_t size() const;
    int sum() const;

    static NumberSet single(int n);
    NumberSet intersection(const NumberSet& a) const;
    NumberSet add(int n) const;
    NumberSet add(const NumberSet& s) const;
    NumberSet remove(const NumberSet& s) const;
    
private:
    int bits_;
};
