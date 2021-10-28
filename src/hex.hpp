#ifndef __HEX_H__
#define __HEX_H__

#include "data.hpp"
#include <string>

namespace LifeHash {

std::string data_to_hex(const Data& in);
Data hex_to_data(const std::string& hex);

}

#endif
