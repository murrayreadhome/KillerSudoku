#pragma once
#include "Cell.h"
#include "SumSet.h"
#include <iosfwd>
#include <string>
#include <deque>

class Board
{
public:
    Board();

    void setup(const std::string& type);
    const std::string& type() const;

    void add_sum(const IdSet& cells, int total, char name);
    void set_cell(size_t cell, int value);
    void add_less_than(int a, int b);

    void apply_rules();
    bool solve();

private:
    void setup_sudoku();
    void setup_lessthan();
    void setup_rowscols(size_t n);

    void link(size_t sum, size_t cell);

    friend std::ostream& operator<<(std::ostream& out, const Board& b);
    friend std::istream& operator>>(std::istream& in, Board& b);

    void restrict_cell(size_t cell, NumberSet allowed);
    void queue_sum(size_t sum);
    void queue_lessthan(size_t cell);
    bool tick();

    enum 
    {
        num_rules = 5,
        num_lessthan_rules = 1
    };
    void sum_restrict_possible(size_t s);
    void restrict_sum_to_cell_possible(size_t s);
    void unique_required_position(size_t s);
    void unique_required_subset(size_t s);
    void unique_required_free_subset(size_t s);
    void lessthan_restrict_possible(size_t lessthan);

    void innie_outie_sums(size_t s);
    void get_non_overlap_innies(size_t s, IdSet& innies);
    void get_non_overlap_outies(const IdSet& cells, IdSet& outies);

    void add_unique_sum(const IdSet& cells, int total, char name);

private:
    std::vector<Cell> cells_;
    std::vector<SumSet> sums_;
    std::vector<std::deque<size_t>> sum_queues_;
    std::vector<int> sum_queued_;
    bool started_ = false;
    size_t n_;
    size_t non_overlap_sums_;
    size_t grid_sums_;
    std::string type_;
    std::vector<std::pair<int,int>> lessthans_;
    std::vector<std::deque<size_t>> lessthan_queues_;
    std::vector<int> lessthan_queued_;
};

std::ostream& operator<<(std::ostream& out, const Board& b);
std::istream& operator>>(std::istream& in, Board& b);
