#ifndef SDP_ENCODER_H
#define SDP_ENCODER_H

#include <utility>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include "sdp.h"
#include "decoder.h"
#include "Array.h"

namespace sdp{
    // An Encoder writes a session description to a buffer.
    struct Encoder  {
        //w       io.Writer
        Array<char> buf;
        
        int64_t  pos;
        bool newline ;


        // NewEncoder returns a new encoder that writes to w.
        // Encoder* NewEncoder(w io.Writer) * {
        //     return new Encoder{w: w}
        // }

        // Encode encodes the session description.
        Error try_Encode(Session* s) ;

        // Reset resets encoder state to be empty.
        void Reset() ;

        Encoder* session(Session* s)  ;

        Encoder* media(Media* m);

        Encoder* format(Format* f);

        Encoder* attr(Attr* a);

        Encoder* timezone(vector<TimeZone*>& z);

        Encoder* timing( Timing* t);

        Encoder* repeat(Repeat* r) ;

        Encoder* time(chrono::time_point<chrono::system_clock> t);

        Encoder*duration(chrono::seconds d);

        Encoder* bandwidth(const string& m, int v);

        Encoder* key(Key* k) ;

        Encoder* origin(Origin* o);

        Encoder* connection(Connection* c) ;

        Encoder* transport(string network, string typ, string addr );

        string strd(string v, string def ) ;

        Encoder* str(const string& v) ;

        Encoder* fields( const string& item1, const string& item2, const string& item3);

        Encoder* sp() ;

        Encoder* set_char(char v);

        Encoder* set_int(int64_t v) ;

        Encoder* add(char n);

        Slice<char> next(int64_t n);

        void grow(int p) ;

        // Bytes returns encoded bytes of the last session description.
        // The bytes stop being valid at the next encoder call.
        Slice<char> Bytes();

        // Bytes returns the encoded session description as str.
        string  String() ;
    };

}

#endif //SDP_ENCODER_H