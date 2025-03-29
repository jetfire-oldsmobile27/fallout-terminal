#pragma once
#include "constants.hpp"
#include <boost/json.hpp>

namespace json = boost::json;

class MenuBuilder {
public:
    static MenuItem buildFromJSON(const json::value& rootValue);
    
private:
    static void parseMenuItem(const json::object& obj, MenuItem& item);
};