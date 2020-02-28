#pragma once
#include <vector>
#include <stddef.h>

// maintains a sorted small set of size_t
class IdSet
{
public:
    IdSet();
    IdSet(const std::vector<size_t>& ids);

    bool operator==(const IdSet& i) const;
    bool operator!=(const IdSet& i) const;

    size_t size() const;
    bool empty() const;

    const size_t* begin() const;
    const size_t* end() const;

    void add(size_t i);
    IdSet add(const IdSet& i) const;
    void remove(size_t i);
    void remove(const IdSet& i);

    size_t overlap(const IdSet& i) const;

private:
    size_t* begin();
    size_t* end();
    size_t* lower_bound(int i);

private:
    size_t size_;
    size_t ids_[9];
};
