#pragma once
#include <vector>
#include <stddef.h>

class NumberSet
{
public:
    NumberSet();
    explicit NumberSet(const std::vector<int>& nums);
    static NumberSet single(int n);
    static NumberSet all();
    static NumberSet none();

    bool operator==(const NumberSet& s) const;
    bool operator!=(const NumberSet& s) const;

    int bits() const;
    const std::vector<int>& nums() const;
    size_t size() const;
    bool empty() const;
    int sum() const;

    NumberSet intersection(const NumberSet& s) const;
    NumberSet add(int n) const;
    NumberSet add(const NumberSet& s) const;
    NumberSet remove(const NumberSet& s) const;
    NumberSet invert() const;

private:
    explicit NumberSet(int bits);

private:
    int bits_;
};
