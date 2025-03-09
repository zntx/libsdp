#include <iostream>
#include <vector>
#include <string>
#include "attribute.h"


    // NewAttr returns a=<attribute>:<value> attribute.
    Attr* Attr::NewAttr(string attr, string value ) {
        return new(std::nothrow) Attr{attr, value};
    }

    // NewAttrFlag returns a=<flag> attribute.
    Attr* Attr::NewAttrFlag(string flag ){
        return new(std::nothrow) Attr{flag, ""};
    }

    string Attr::String() {
        if( this->Value == "") {
            return this->Name;
        }
        return this->Name + ":" + this->Value;
    } 

    Attributes::Attributes()
    {
        
    }
    Attributes::~Attributes()
    {
    }

    // Has returns presence of attribute by name.
    bool Attributes::Has(string name)
    {
        for ( auto && it : this->attr ){
            if (it->Name == name) {
                return true;
            }
        }
        return false;
    }

    // Get returns first attribute value by name.
    string Attributes::Get(string name)
    {
        for ( auto && it : this->attr ){
            if (it->Name == name) {
                return it->Value;
            }
        }
        return "";
    }
