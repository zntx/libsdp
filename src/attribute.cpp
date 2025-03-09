#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include "attribute.h"

namespace sdp{
    // NewAttr returns a=<attribute>:<value> attribute.
    Attr* Attr::NewAttr(string attr, string value ) {
        return new(std::nothrow) Attr{std::move(attr), std::move(value)};
    }

    // NewAttrFlag returns a=<flag> attribute.
    Attr* Attr::NewAttrFlag(string flag ){
        return new(std::nothrow) Attr{std::move(flag), ""};
    }

    string Attr::String() const {
        if( this->Value.empty()) {
            return this->Name;
        }
        return this->Name + ":" + this->Value;
    } 


    // Has returns presence of attribute by name.
    bool Attributes::Has(const string& name)
    {
        for ( auto && it : this->attr ){
            if (it->Name == name) {
                return true;
            }
        }
        return false;
    }

    // Get returns first attribute value by name.
    string Attributes::Get(const string& name)
    {
        for ( auto && it : this->attr ){
            if (it->Name == name) {
                return it->Value;
            }
        }
        return "";
    }
}
