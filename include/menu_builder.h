#pragma once
#include "constants.hpp"
#include <boost/json.hpp>

namespace json = boost::json;

class MenuBuilder {
public:
    static MenuItem build_from_json(const json::value& root_value);
    
private:
    static void parse_menu_item(const json::object& obj, MenuItem& item);
};