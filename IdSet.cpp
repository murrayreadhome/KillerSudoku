#include "IdSet.h"
#include <algorithm>
using namespace std;

IdSet::IdSet()
    : size_(0)
{
}

IdSet::IdSet(const vector<size_t>& ids)
{
    copy(ids.begin(), ids.end(), begin());
    size_ = ids.size();
    sort(begin(), end());
}

bool IdSet::operator==(const IdSet& i) const
{
    if (size_ != i.size_)
        return false;
    return equal(begin(), end(), i.begin());
}

bool IdSet::operator!=(const IdSet& i) const
{
    return !(*this == i);
}

size_t IdSet::size() const
{
    return size_;
}

bool IdSet::empty() const
{
    return size_ == 0;
}

const size_t* IdSet::begin() const
{
    return ids_;
}

const size_t* IdSet::end() const
{
    return ids_+size_;
}

size_t* IdSet::begin()
{
    return ids_;
}

size_t* IdSet::end()
{
    return ids_+size_;
}

size_t* IdSet::lower_bound(int i)
{
    size_t* at = begin();
    size_t* stop = end();
    while (at != stop && *at < i)
        at++;
    return at;
}

void IdSet::add(size_t i)
{
    size_t* at = lower_bound(i);
    if ((at == end() || *at != i) && size_<9)
    {
        size_t* m = end();
        while (m != at)
        {
            *m = m[-1];
            --m;
        }
        *at = i;
        size_++;
    }
}

IdSet IdSet::add(const IdSet& i) const
{
    IdSet out;
    auto b1 = begin();
    auto e1 = end();
    auto b2 = i.begin();
    auto e2 = i.end();
    while ((b1 != e1 || b2 != e2) && out.size_ != 9)
    {
        if (b1 != e1 && b2 != e2 && *b1 == *b2)
        {
            out.ids_[out.size_++] = *b1;
            b1++;
            b2++;
        }
        else if (b1 != e1 && (b2 == e2 || *b1 < *b2))
        {
            out.ids_[out.size_++] = *b1;
            b1++;
        }
        else
        {
            out.ids_[out.size_++] = *b2;
            b2++;
        }
    }
    return out;
}

void IdSet::remove(size_t i)
{
    auto r = begin();
    auto w = r;
    auto e = end();
    while (r != e)
    {
        if (*r != i)
            *w++ = *r;
        else
            size_--;
        r++;
    }
}

void IdSet::remove(const IdSet& i)
{
    auto r = begin();
    auto w = r;
    auto e = end();
    auto r2 = i.begin();
    auto e2 = i.end();
    while (r != e)
    {
        while (r2 != e2 && *r2 < *r)
            r2++;
        if (r2 != e2 && *r2 == *r)
            size_--;
        else
            *w++ = *r;
        r++;
    }
}

size_t IdSet::overlap(const IdSet& i) const
{
    auto r1 = begin();
    auto e1 = end();
    auto r2 = i.begin();
    auto e2 = i.end();
    size_t count = 0;
    while (r1 != e2 && r2 != e2)
    {
        while (r2 != e2 && *r2 < *r1)
            r2++;
        if (r2 != e2 && *r2 == *r1)
            count++;
        r1++;
    }
    return count;
}

#include "gtest/gtest.h"

TEST(IdSet, Ctor)
{
    const IdSet empty;
    EXPECT_EQ(0u, empty.size());
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.begin(), empty.end());

    const IdSet s({2,1});
    EXPECT_EQ(2u, s.size());
    EXPECT_FALSE(s.empty());
    EXPECT_NE(s.begin(), s.end());
    EXPECT_EQ(1u, *s.begin());
    EXPECT_EQ(2u, *(s.begin()+1));
    EXPECT_EQ(s.begin()+2, s.end());
}

TEST(IdSet, Add1)
{
    IdSet s;
    s.add(3);
    EXPECT_EQ(IdSet({3}), s);
    s.add(1);
    EXPECT_EQ(IdSet({1,3}), s);
    s.add(2);
    EXPECT_EQ(IdSet({1,2,3}), s);
    s.add(4);
    EXPECT_EQ(IdSet({1,2,3,4}), s);
    s.add(2);
    EXPECT_EQ(IdSet({1,2,3,4}), s);
}

TEST(IdSet, AddMany)
{
    EXPECT_EQ(IdSet(), IdSet().add(IdSet()));
    EXPECT_EQ(IdSet({1}), IdSet({1}).add(IdSet()));
    EXPECT_EQ(IdSet({1}), IdSet().add(IdSet({1})));
    EXPECT_EQ(IdSet({1}), IdSet({1}).add(IdSet({1})));
    EXPECT_EQ(IdSet({1,2}), IdSet({1}).add(IdSet({2})));
    EXPECT_EQ(IdSet({1,2,3,4}), IdSet({1,3}).add(IdSet({2,4})));
    EXPECT_EQ(IdSet({1,2,3,4}), IdSet({2,4}).add(IdSet({1,3})));
    EXPECT_EQ(IdSet({1,2,3}), IdSet({2,3}).add(IdSet({1,2})));
}

TEST(IdSet, Remove1)
{
    IdSet s({1,2,3,4});
    s.remove(2);
    EXPECT_EQ(IdSet({1,3,4}), s);
    s.remove(2);
    EXPECT_EQ(IdSet({1,3,4}), s);
    s.remove(4);
    EXPECT_EQ(IdSet({1,3}), s);
    s.remove(1);
    EXPECT_EQ(IdSet({3}), s);
    s.remove(3);
    EXPECT_EQ(IdSet(), s);
    s.remove(3);
    EXPECT_EQ(IdSet(), s);
}

TEST(IdSet, RemoveMany)
{
    IdSet s({1,2,3,4});
    s.remove(IdSet({6,7,8}));
    EXPECT_EQ(IdSet({1,2,3,4}), s);
    s.remove(IdSet({0,2,3,5}));
    EXPECT_EQ(IdSet({1,4}), s);
    s.remove(IdSet({1,4}));
    EXPECT_EQ(IdSet(), s);
}

TEST(IdSet, Overlap)
{
    EXPECT_EQ(0, IdSet().overlap(IdSet()));
    EXPECT_EQ(0, IdSet({1}).overlap(IdSet({2})));
    EXPECT_EQ(1, IdSet({1}).overlap(IdSet({1})));
    EXPECT_EQ(3, IdSet({1,2,4,8}).overlap(IdSet({1,2,3,5,8})));
    EXPECT_EQ(1, IdSet({1,2,4,8}).overlap(IdSet({0,2,3,5,9})));
}
