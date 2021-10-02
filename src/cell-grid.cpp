#include "cell-grid.hpp"
#include "bit-enumerator.hpp"
#include "bit-aggregator.hpp"
#include <assert.h>

namespace LifeHash
{

CellGrid::CellGrid(const Size& size)  : Grid(size) {}

Data CellGrid::data() const {
    auto a = BitAggregator();
    for_all([&](const Point& p) {
        a.append(get_value(p));
    });
    return a.data();
}

void CellGrid::set_data(const Data& data) {
    assert(capacity == data.size() * 8);
    auto e = BitEnumerator(data);
    auto i = storage.begin();
    e.for_all([&](bool b) {
        *i = b;
        i++;
    });
    assert(i == storage.end());
}

} // namespace LifeHash
