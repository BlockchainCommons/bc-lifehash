#ifndef LIFEHASH_CELL_GRID_HPP
#define LIFEHASH_CELL_GRID_HPP

#include "change-grid.hpp"
#include "data.hpp"
#include "grid.hpp"

namespace LifeHash
{

// A class that holds an array of boolean cells and that is
// capable of running Conway's Game of Life to produce the
// next generation.
class CellGrid : public Grid<bool> {
   public:
    CellGrid(const Size& size);

   private:
    inline static bool is_alive_in_next_generation(bool currentAlive, size_t neighborsCount) {
        if (currentAlive) {
            return neighborsCount == 2 || neighborsCount == 3;
        } else {
            return neighborsCount == 3;
        }
    }

    inline std::size_t count_neighbors(const Point& point) {
        size_t total = 0;
        for_neighborhood(point, [&](const Point& o, const Point& p) {
            if (o == Point::zero) {
                return;
            }
            if (this->get_value(p)) {
                total++;
            }
        });
        return total;
    }

   public:
    Data data() const;
    void set_data(const Data& data);

    void next_generation(const ChangeGrid& currentChangeGrid, CellGrid& nextCellGrid, ChangeGrid& nextChangeGrid) {
        nextCellGrid.set_all(false);
        nextChangeGrid.set_all(false);
        for_all([&](const Point& p) {
            auto currentAlive = this->get_value(p);
            if (currentChangeGrid.get_value(p)) {
                auto neighborsCount = this->count_neighbors(p);
                auto nextAlive = is_alive_in_next_generation(currentAlive, neighborsCount);
                if (nextAlive) {
                    nextCellGrid.set_value(true, p);
                }
                if (currentAlive != nextAlive) {
                    nextChangeGrid.set_changed(p);
                }
            } else {
                nextCellGrid.set_value(currentAlive, p);
            }
        });
    }

    virtual Color color_for_value(const bool& value) const { return value ? Color::white : Color::black; }
};

} // namespace LifeHash

#endif
