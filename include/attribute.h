#include <iostream>
#include <vector>
#include <string>

namespace sdp{

    using namespace std;

    // Session or media attribute values for indication of a streaming mode.
    #define	ModeSendRecv "sendrecv"
    #define ModeRecvOnly "recvonly"
    #define ModeSendOnly "sendonly"
    #define ModeInactive "inactive"

    // Attr represents session or media attribute.
    struct Attr  {
        string Name;
        string Value;

        
        // NewAttr returns a=<attribute>:<value> attribute.
        static Attr* NewAttr(string attr, string value );

        // NewAttrFlag returns a=<flag> attribute.
        static Attr* NewAttrFlag(string flag );

        string String() const;
    };

    struct Attributes {
        vector<Attr*> attr;

        Attributes() = default;
        ~Attributes() = default;
        // Has returns presence of attribute by name.
        bool Has(const string& name);

        // Get returns first attribute value by name.
        string Get(const string& name);
    };
}