#include "menu_builder.h"

MenuItem MenuBuilder::build_from_json(const json::value& root_value) {
    MenuItem root;
    const auto& root_obj = root_value.as_object();

    if (auto it = root_obj.find("name"); it != root_obj.end()) {
        root.name = json::value_to<std::string>(it->value());
    }

    if (auto it = root_obj.find("submenu"); it != root_obj.end()) {
        const auto& arr = it->value().as_array();
        root.submenu.reserve(arr.size());
        
        for (const auto& item : arr) {
            MenuItem child;
            parse_menu_item(item.as_object(), child);
            root.submenu.push_back(std::move(child));
        }
    }

    return root;
}

void MenuBuilder::parse_menu_item(const json::object& obj, MenuItem& item) {
    if(obj.contains("name")) {
        item.name = json::value_to<std::string>(obj.at("name"));
    }
    
    if(obj.contains("function")) {
        item.function = json::value_to<std::string>(obj.at("function"));
    }
    
    if(obj.contains("submenu")) {
        for(const auto& subitem : obj.at("submenu").as_array()) {
            MenuItem child;
            parse_menu_item(subitem.as_object(), child);
            item.submenu.push_back(child);
        }
    }
}