#include "Board.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <map>
#include <sstream>
using namespace std;


Board::Board()
{}

void Board::setup(const string& type)
{
    type_ = type;
    if (type == "sudoku" || type == "killer")
        setup_sudoku();
    else if (type == "lessthan")
        setup_lessthan();
}

const string& Board::type() const
{
    return type_;
}

void Board::setup_sudoku()
{
    n_ = 9;
    grid_sums_ = 27;
    sums_.resize(grid_sums_, SumSet(45, 9));
    sum_queued_.resize(grid_sums_, 0);
    setup_rowscols(9);

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

void Board::setup_lessthan()
{
    n_ = 6;
    grid_sums_ = 12;
    sums_.resize(grid_sums_, SumSet(21, 6));
    sum_queued_.resize(grid_sums_, 0);
    setup_rowscols(6);
}

void Board::setup_rowscols(size_t n)
{
    cells_.resize(n*n);
    sum_queues_.resize(num_rules);
    lessthan_queues_.resize(num_lessthan_rules);
    lessthan_queued_.resize(n*n, 0);

    for (size_t i=0; i<cells_.size(); i++)
        cells_[i].id = i;
    for (size_t i=0; i<sums_.size(); i++)
    {
        sums_[i].id = i;
        queue_sum(i);
    }

    // rows
    for (size_t y=0; y<n; y++)
    {
        for (size_t x=0; x<n; x++)
        {
            size_t cell = y*n + x;
            link(y, cell);
        }
    }
    
    // columns
    for (size_t y=0; y<n; y++)
    {
        for (size_t x=0; x<n; x++)
        {
            size_t col = x+n;
            size_t cell = y*n + x;
            link(col, cell);
        }
    }
}

void Board::add_sum(const IdSet& cells, int total, char name)
{
    if (cells.empty())
        return;
    SumSet s(total, cells.size());
    s.cells = cells;
    s.id = sums_.size();
    s.name = name;
    sums_.push_back(s);
    sum_queued_.push_back(0);
    for (size_t cell : cells)
        cells_[cell].sums.add(s.id);
    queue_sum(s.id);
}

void Board::add_less_than(int a, int b)
{
    size_t id = lessthans_.size();
    lessthans_.push_back({a,b});
    lessthan_queued_.push_back(0);
    cells_[a].lessthans.add(id);
    cells_[b].lessthans.add(id);
    queue_lessthan(id);
}

void Board::add_unique_sum(const IdSet& cells, int total, char name)
{
    for (const SumSet& s : sums_)
        if (s.cells == cells)
            return;
    add_sum(cells, total, name);
}

void Board::set_cell(size_t cell, int value)
{
    Cell& c = cells_[cell];
    c.set_value(value);
    for (size_t s : c.sums)
    {
        sums_[s].cells.remove(cell);
        sums_[s].remove_cell(value);
        queue_sum(s);
    }
    for (size_t l : c.lessthans)
        queue_lessthan(l);
}

void Board::restrict_cell(size_t cell, NumberSet allowed)
{
    Cell& c = cells_[cell];
    NumberSet was = c.numbers();
    NumberSet restriction = was.intersection(allowed);
    if (restriction == was)
        return;
    c.set_numbers(restriction);
    if (c.value() != Unknown)
        set_cell(cell, c.value());
    for (size_t s : c.sums)
        queue_sum(s);
    for (size_t l: c.lessthans)
        queue_lessthan(l);
}

void Board::queue_sum(size_t sum)
{
    for (size_t i=0; i<num_rules; i++)
    {
        int queue_flag = 1<<i;
        int& sum_flags = sum_queued_[sum];
        if (sum_flags & queue_flag)
            continue;
        sum_flags |= queue_flag;
        sum_queues_[i].push_back(sum);
    }
}

void Board::queue_lessthan(size_t cell)
{
    for (size_t i=0; i<num_lessthan_rules; i++)
    {
        int queue_flag = 1<<i;
        int& cell_flags = lessthan_queued_[cell];
        if (cell_flags & queue_flag)
            continue;
        cell_flags |= queue_flag;
        lessthan_queues_[i].push_back(cell);
    }
}

bool Board::tick()
{
    for (size_t i=0; i<num_lessthan_rules; i++)
    {
        auto& queue = lessthan_queues_[i];
        if (queue.empty())
            continue;
        size_t lessthan = queue.back();
        queue.pop_back();
        switch (i)
        {
            case 0:
                lessthan_restrict_possible(lessthan);
                break;
        }
        int queue_flag = 1<<i;
        int& lessthan_flags = lessthan_queued_[lessthan];
        lessthan_flags &= ~queue_flag;
        return true;
    }
    for (size_t i=0; i<num_rules; i++)
    {
        auto& queue = sum_queues_[i];
        if (queue.empty())
            continue;
        size_t sum = queue.back();
        queue.pop_back();
        switch (i)
        {
            case 0:
                sum_restrict_possible(sum);
                break;
            case 1:
                restrict_sum_to_cell_possible(sum);
                break;
            case 2:
                unique_required_position(sum);
                break;
            case 3:
                unique_required_subset(sum);
                break;
            case 4:
                unique_required_free_subset(sum);
                break;
        }
        int queue_flag = 1<<i;
        int& sum_flags = sum_queued_[sum];
        sum_flags &= ~queue_flag;
        return true;
    }
    return false;
}

void Board::lessthan_restrict_possible(size_t lessthan)
{
    const auto& lt = lessthans_[lessthan];
    Cell& small = cells_[lt.first];
    Cell& big = cells_[lt.second];

    int low = small.numbers().nums().front() + 1;
    NumberSet high_allowed;
    for (int i=low; i<=n_; i++)
        high_allowed = high_allowed.add(i);
    restrict_cell(big.id, high_allowed);

    int high = big.numbers().nums().back() - 1;
    NumberSet low_allowed;
    for (int i=1; i<=high; i++)
        low_allowed = low_allowed.add(i);
    restrict_cell(small.id, low_allowed);
}

void Board::unique_required_free_subset(size_t s)
{
    const SumSet& parent = sums_[s];
    size_t n = parent.cells.size();
    for (int i=0; i<(1<<n); i++)
    {
        NumberSet ns(i);
        size_t nb = ns.size();
        if (nb < 2 || n-2 < nb)
            continue;
        NumberSet subset;
        for (int b : ns.nums())
            subset = subset.add(cells_[parent.cells.begin()[b-1]].numbers());
        if (subset.size() != nb)
            continue;
        NumberSet allowed = subset.invert();
        for (size_t j=n-1; j<n; j--)
        {
            if (ns.contains(j+1))
                continue;
            restrict_cell(parent.cells.begin()[j], allowed);
        }
    }
}

void Board::unique_required_subset(size_t s)
{
    IdSet innies;
    const SumSet& parent = sums_[s];
    for (size_t c : parent.cells)
    {
        for (size_t i : cells_[c].sums)
        {
            if (i == s)
                continue;
            const SumSet& innie = sums_[i];
            if (innie.num_cells() > 0 &&
                innie.num_cells() < parent.num_cells() &&
                parent.cells.overlap(innie.cells) == innie.cells.size())
                innies.add(i);
        }
    }

    for (size_t i : innies)
    {
        const SumSet& innie = sums_[i];
        if (innie.required().empty())
            continue;
        NumberSet allowed = innie.required().invert();
        IdSet others = parent.cells;
        others.remove(innie.cells);
        for (size_t c : others)
            restrict_cell(c, allowed);
    }
}

void Board::innie_outie_sums(size_t s)
{
    if (s >= grid_sums_)
        return;
    IdSet innies;
    get_non_overlap_innies(s, innies);
    if (innies.empty())
        return;
    const SumSet& parent = sums_[s];
    IdSet remainder = parent.cells;
    int diff = parent.total();
    for (size_t id : innies)
    {
        remainder.remove(sums_[id].cells);
        diff -= sums_[id].total();
    }
    if (diff <= 0)
        return;
    add_unique_sum(remainder, diff, '.');

    IdSet outies;
    get_non_overlap_outies(remainder, outies);
    if (outies.size() > 1)
        return; // TODO add SumGroup
    const SumSet& outie = sums_[*outies.begin()];
    IdSet outie_cells = outie.cells;
    outie_cells.remove(remainder);
    add_unique_sum(outie_cells, outie.total() - diff, '.');
}

void Board::get_non_overlap_innies(size_t s, IdSet& innies)
{
    const SumSet& parent = sums_[s];
    for (size_t c : parent.cells)
    {
        for (size_t i : cells_[c].sums)
        {
            if (i == s)
                continue;
            if (i < grid_sums_ || non_overlap_sums_ <= i)
                continue;
            const SumSet& innie = sums_[i];
            if (parent.cells.overlap(innie.cells) == innie.cells.size())
                innies.add(i);
        }
    }
}

void Board::get_non_overlap_outies(const IdSet& cells, IdSet& outies)
{
    for (size_t c : cells)
    {
        for (size_t i : cells_[c].sums)
        {
            if (i < grid_sums_ || non_overlap_sums_ <= i)
                continue;
            const SumSet& outie = sums_[i];
            if (cells.overlap(outie.cells) < outie.cells.size())
                outies.add(i);
        }
    }
}

void Board::restrict_sum_to_cell_possible(size_t s)
{
    SumSet& sum = sums_[s];
    NumberSet possible = sum.possible();
    NumberSet cell_possible;
    for (size_t c : sum.cells)
        cell_possible = cell_possible.add(cells_[c].numbers());
    cell_possible = cell_possible.intersection(possible);
    if (possible != cell_possible)
    {
        sum.restrict_possible(cell_possible);
        unique_required_position(s);
    }
}

void Board::unique_required_position(size_t s)
{
    const SumSet& sum = sums_[s];
    NumberSet required = sum.required();
    if (required.empty())
        return;
    for (size_t c : sum.cells)
    {
        NumberSet unique = cells_[c].numbers().intersection(required);
        if (unique.empty())
            continue;
        for (size_t d : sum.cells)
        {
            if (c == d)
                continue;
            unique = unique.intersection(cells_[d].numbers().invert());
        }
        if (!unique.empty())
            restrict_cell(c, unique);
    }
}

void Board::sum_restrict_possible(size_t s)
{
    const SumSet& sum = sums_[s];
    NumberSet possible = sum.possible();
    for (size_t c : sum.cells)
        restrict_cell(c, possible);
}

void Board::apply_rules()
{
    if (!started_)
    {
        non_overlap_sums_ = sums_.size();
        for (size_t s=0; s<grid_sums_; s++)
            innie_outie_sums(s);
    }
    started_ = true;
    bool keep_going = true;
    while (keep_going)
        keep_going = tick();
}

bool Board::solve()
{
    size_t not_done = 81;
    try
    {
        apply_rules();
        for (size_t i=0; i<81; i++)
        {
            if (cells_[i].value() == Unknown)
            {
                not_done = i;
                break;
            }
        }
        if (not_done == 81)
            return true;
        for (int n : cells_[not_done].numbers().nums())
        {
            Board b = *this;
            b.set_cell(not_done, n);
            if (b.solve())
            {
                *this = b;
                return true;
            }
        }
        return false;
    }
    catch(const std::runtime_error& e)
    {
        return false;
    }
}

void Board::link(size_t sum, size_t cell)
{
    sums_[sum].cells.add(cell);
    cells_[cell].sums.add(sum);
}

std::ostream& operator<<(std::ostream& out, const Board& b)
{
    out << b.type() << endl;
    set<size_t> sums_used;
    set<size_t> lessthans_used;
    for (size_t y=0; y<b.n_; y++)
    {
        for (size_t x=0; x<b.n_; x++)
        {
            const Cell& cell = b.cells_[y*b.n_ + x];
            if (cell.value() != Unknown)
                out << cell.value();
            else
            {
                const size_t* psum_id = find_if(
                    cell.sums.begin(),
                    cell.sums.end(),
                    [&](size_t s){return b.sums_[s].name != '.';});
                if (psum_id != cell.sums.end())
                {
                    sums_used.insert(*psum_id);
                    out << b.sums_[*psum_id].name;
                }
                else
                {
                    out << ".";
                }
                for (size_t l : cell.lessthans)
                    lessthans_used.insert(l);
            }
        }
        out << endl;
    }
    for (size_t sum_id : sums_used)
    {
        const SumSet& sum = b.sums_[sum_id];
        out << sum.name << " " << sum.total() << endl;
    }
    for (size_t l : lessthans_used)
    {
        const auto& lt = b.lessthans_[l];
        out << lt.first << "<" << lt.second << endl;
    }
}

std::istream& operator>>(std::istream& in, Board& b)
{
    string type;
    in >> type; in.get();
    b.setup(type);
    map<char, IdSet> sum_cells;
    for (size_t y=0; y<b.n_; y++)
    {
        for (size_t x=0; x<b.n_; x++)
        {
            size_t cell = y*b.n_ + x;
            char c;
            in >> c;
            if ('1'<=c && c<='9')
                b.set_cell(cell, c-'0');
            else
                sum_cells[c].add(cell);
        }
        in.get();
    }
    if (type == "killer")
    {
        for (size_t i=0; i<sum_cells.size(); i++)
        {
            if (in.eof())
                break;
            char c;
            int t;
            in >> c;
            if (in.eof())
                break;
            in >> t;
            in.get();
            if (sum_cells.count(c))
                b.add_sum(sum_cells[c], t, c);
        }
    }
    else if (type == "lessthan")
    {
        while (!in.eof())
        {
            string line;
            getline(in, line);
            if (line.size() >= 3)
            {
                const IdSet& from = sum_cells[line[0]];
                const IdSet& to = sum_cells[line[2]];
                char op = line[1];
                if (from.size() && to.size() && (op=='<' || op=='>'))
                {
                    int x = *from.begin();
                    int y = *to.begin();
                    if (op=='>')
                        swap(x, y);
                    b.add_less_than(x, y);
                }
            }
        }
    }
}

#include "gtest/gtest.h"

TEST(Board, Sudoku)
{
    string sudoku = R"(sudoku
53..7....
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
    b.apply_rules();
    cout << b;
}

TEST(Board, KillerSudoku)
{
    string sudoku = R"(killer
ABCCDEFFG
ABCCDEEGG
HIIIJKLMG
HNNOJKLMP
QRROSTTUP
QQRSSVWUP
XYZaVVWUb
XYZaccddb
YYeeffddd
A 14
B 12
C 22
D 4
E 16
F 11
G 14
H 8
I 20
J 14
K 11
L 13
M 9
N 9
O 7
P 18
Q 11
R 18
S 12
T 9
U 15
V 22
W 4
X 7
Y 18
Z 15
a 10
b 7
c 13
d 31
e 8
f 3
)";
    stringstream in(sudoku);
    Board b;
    in >> b;
    b.apply_rules();
    cout << b;
}

TEST(Board, KillerSudoku2)
{
    string sudoku = R"(killer
AABCDEFFG
HIBCDEJJG
HIKKLLJJM
NNKKOLPQM
RRSTOPPQU
VWSTXYYUU
VWZaXbccd
VeZafbggd
eefffbddd
A 10
B 14
C 13
D 10
E 11
F 8
G 12
H 5
I 10
J 22
K 22
L 19
M 4
N 8
O 6
P 10
Q 9
R 15
S 6
T 13
U 18
V 21
W 3
X 11
Y 11
Z 8
a 5
b 19
c 15
d 23
e 18
f 19
g 7
)";
    stringstream in(sudoku);
    Board b;
    in >> b;
    b.solve();
    cout << b;
}

TEST(Board, LessThan)
{
    string sudoku = R"(lessthan
AB1CDE
FG....
HIJKL.
M..NO.
....5.
.4PQRS
A>F
B>G
F<G
C>D
D>E
H>M
H<I
J<K
K<N
L>O
P<Q
R>S
)";
    stringstream in(sudoku);
    Board b;
    in >> b;
    b.apply_rules();
    cout << b;
}
