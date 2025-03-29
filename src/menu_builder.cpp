#include "menu_builder.h"

MenuItem MenuBuilder::buildFromJSON(const json::value& rootValue) {
    MenuItem root;
    const auto& rootObj = rootValue.as_object();
    
    if(rootObj.contains("title")) {
        root.name = json::value_to<std::string>(rootObj.at("title"));
    }
    
    if(rootObj.contains("items")) {
        for(const auto& item : rootObj.at("items").as_array()) {
            MenuItem child;
            parseMenuItem(item.as_object(), child);
            root.submenu.push_back(child);
        }
    }
    
    return root;
}

void MenuBuilder::parseMenuItem(const json::object& obj, MenuItem& item) {
    if(obj.contains("name")) {
        item.name = json::value_to<std::string>(obj.at("name"));
    }
    
    if(obj.contains("function")) {
        item.function = json::value_to<std::string>(obj.at("function"));
    }
    
    if(obj.contains("submenu")) {
        for(const auto& subitem : obj.at("submenu").as_array()) {
            MenuItem child;
            parseMenuItem(subitem.as_object(), child);
            item.submenu.push_back(child);
        }
    }
}