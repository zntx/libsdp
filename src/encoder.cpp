#include <utility>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include "sdp.h"
#include "decoder.h"
#include "encoder.h"



    // Encode encodes the session description.
    Error Encoder::try_Encode(Session* s) 
    {
        this->Reset();
        this->session(s);
        // if (this->w != nullptr) {
        //     _, err := this->w.Write(this->Bytes());
        //     if (err != nullptr) {
        //         return err;
        //     }
        // }
        return nullptr;
    }

    // Reset resets encoder state to be empty.
    void Encoder::Reset() 
    {
        this->pos, this->newline = 0, false;
    }

    Encoder* Encoder::session(Session* s)  
    {
        this->add('v')->set_int(s->Version);

        if (s->origin != nullptr) {
            this->add('o')->origin(s->origin);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.subslice(0, this->pos).to_string() << std::endl;

        this->add('s')->str(s->Name);
        if (s->Information != "") {
            this->add('i')->str(s->Information);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        if (s->URI != "") {
            this->add('u')->str(s->URI);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        for( auto &&it : s->Email) {
            this->add('e')->str(it);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        for ( auto &&it :  s->Phone ){
            this->add('p')->str(it);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        if (s->connection != nullptr) {
            this->add('c')->connection(s->connection);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        for (auto &&it : s->bandwidth) {
            this->add('b')->bandwidth(it.first, it.second);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        if ( s->timeZone.size() > 0) {
            this->add('z')->timezone(s->timeZone);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        for (auto &&it : s->key) {
            this->add('k')->key(it);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        this->add('t')->timing(s->timing);
        for ( auto &&it : s->repeat ) {
            this->add('r')->repeat(it);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        if( s->Mode != "") {
            this->add('a')->str(s->Mode);
        }

        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        for ( auto &&it : s->attributes.attr ) {
            this->add('a')->attr(it);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        for ( auto &&it : s->media ) {
            this->media(it);
        }
        std::cout << "Encoder::session() slice.to_string() = " << this->buf.to_string() << std::endl;

        return this;
    }

    Encoder* Encoder::media(Media* m)
    {
        this->add('m')->str(m->Type)->sp()->set_int(m->Port);
        if (m->PortNum > 0) {
            this->set_char('/')->set_int( m->PortNum);
        }
        this->sp()->str(m->Proto);
        for (auto &&it : m->formats) {
            this->sp()->set_int( it->Payload);
        }
        if (m->formats.size() == 0) {
            this->sp()->set_char('*');
        }
        if (m->Information != "") {
            this->add('i')->str(m->Information);
        }
        for (  auto &&it : m->connection ) {
            this->add('c')->connection(it);
        }
        for ( auto &&it : m->bandwidth ) {
            this->add('b')->bandwidth(it.first, it.second);
        }
        for (auto &&it : m->key) {
            this->add('k')->key(it);
        }
        for (auto &&it : m->formats) {
            this->format(it);
        }
        if( m->Mode != "") {
            this->add('a')->str(m->Mode);
        }
        for ( auto &&it : m->attributes.attr ) {
            this->add('a')->attr(it);
        }
        return this;
    }

    Encoder* Encoder::format(Format* f)
    {
        auto p = f->Payload;
        if (f->Name != "") {
            this->add('a')->str("rtpmap:")->set_int(p)->sp()->str(f->Name)->set_char('/')->set_int(f->ClockRate);
            if (f->Channels > 0) {
                this->set_char('/')->set_int( f->Channels);
            }
        }
        for (auto &&it : f->Feedback) {
            this->add('a')->str("rtcp-fb:")->set_int(p)->sp()->str(it);
        }
        for (const auto& it :  f->Params) {
            this->add('a')->str("fmtp:")->set_int(p)->sp()->str(it);
        }
        return this;
    }

    Encoder* Encoder::attr(Attr* a)
    {
        if (a->Value == "") {
            return this->str(a->Name);
        }
        return this->str(a->Name)->set_char(':')->str(a->Value);
    }

    Encoder* Encoder::timezone(vector<TimeZone*>& z) 
    {   
        auto i = 0;
        for ( auto && it : z) {
            i++;
            if (i > 1) {
                this->set_char(' ');
            }
            this->time(it->Time)->sp()->duration(it->Offset);
        }
        return this;
    }

    Encoder* Encoder::timing( Timing* t)
    {
        if (t == nullptr) {
            return this->str("0 0");
        }
        return this->time(t->Start)->sp()->time(t->Stop);
    }

    Encoder* Encoder::repeat(Repeat* r) 
    {
        this->duration(r->Interval)->sp()->duration(r->Duration);
        for (auto &&it :  r->Offsets) {
            this->sp()->duration(it);
        }
        return this;
    }

    Encoder* Encoder::time(chrono::time_point<chrono::system_clock> t)
    {
        if (t == chrono::system_clock::time_point() )  {
            return this->set_char('0');
        }
        //return this->int(int64(t.Sub(epoch).Seconds()));

        auto durationSinceEpoch = t.time_since_epoch();
        return this->set_int(    chrono::duration_cast<chrono::seconds>(durationSinceEpoch).count());
    }

    Encoder* Encoder::duration(chrono::seconds d)
    {
        auto v = d.count();
        
        if( v == 0 )
            return this->set_char('0');
        else if( v%86400 == 0)
            return this->set_int(v / 86400)->set_char('d');
        else if( v%3600 == 0)
            return this->set_int(v / 3600)->set_char('h');
        else if( v%60 == 0)
            return this->set_int(v / 60)->set_char('m');
        else 
            return this->set_int(v);
    }

    Encoder* Encoder::bandwidth(string m, int v)
    {
        return this->str(m)->set_char(':')->set_int( v );
    }

    Encoder* Encoder::key(Key* k) 
    {
        if (k->Value == "") {
            return this->str(k->Method);
        }
        return this->str(k->Method)->set_char(':')->str(k->Value);
    }

    Encoder* Encoder::origin(Origin* o)
    {
        return this->str(strd(o->Username, "-"))->sp()->set_int(o->SessionID)
                                                ->sp()->set_int(o->SessionVersion)
                                                ->sp()->transport(o->Network, o->Type, o->Address);
    }

    Encoder* Encoder::connection(Connection* c) 
    {
        this->transport(c->Network, c->Type, c->Address);
        if (c->TTL > 0) {
            this->set_char('/')->set_int( c->TTL );
        }
        if (c->AddressNum > 1) {
            this->set_char('/')->set_int( c->AddressNum);
        }
        return this;
    }

    Encoder* Encoder::transport(string network, string typ, string addr )
    {
        return this->fields(strd(std::move(network), "IN"), strd(std::move(typ), "IP4"), strd(std::move(addr), "127.0.0.1"));
    }

    string Encoder::strd(string v, string def ) 
    {
        if (v.empty()) {
            return def;
        }
        return v;
    }

    Encoder* Encoder::str(string v) 
    {
        if (v == "") {
            return this->set_char('-');
        }
        //copy(this->next(len(v)), v);
        this->next( v.size()).copy(v.c_str(), v.size());

        return this;
    }

    // Encoder*  fields(v ...string)  
    // {
    //     n := len(v) - 1
    //     for _, it := range v {
    //         n += len(it)
    //     }
    //     p, b := 0, this->next(n)
    //     for _, it := range v {
    //         if p > 0 {
    //             b[p] = ' '
    //             p++
    //         }
    //         p += copy(b[p:], it)
    //     }
    //     return e
    // }

    Encoder* Encoder::fields( string item1, string item2, string item3)
    {
        this->next( item1.size() ).copy(item1.c_str(), item1.size());
        this->next( 1 ).set(0,  ' ');
        this->next( item2.size() ).copy(item2.c_str(), item2.size());
        this->next( 1 ).set(0,  ' ');
        this->next( item3.size() ).copy(item3.c_str(), item3.size());
        this->next( 1 ).set(0,  ' ');

        return this;
    }

    Encoder* Encoder::sp() 
    {
        return this->set_char(' ');
    }

    Encoder* Encoder::set_char(char v)
    {
        this->next(1)[0] = v;
        return this;
    }

    Encoder* Encoder::set_int(int64_t v) 
    {
        auto b = this->next(20);
        //this->pos += len(strconv.AppendInt(b[:0], v, 10)) - len(b);
        auto vstr = std::to_string(v);
        for(auto index = 0; index < vstr.size(); index++) {
            b[index] = vstr[index];
        }
        this->pos += vstr.size() - b.size();

        return this;
    }

    Encoder* Encoder::add(char n)
    {
        if (this->newline ){
            auto b = this->next(4);
            b[0] = '\r';
            b[1] = '\n';
            b[2] =  n;
            b[3] = '=';
        } 
        else {
            auto b = this->next(2);
            b[0] = n;
            b[1] = '=';
            this->newline = true;
        }

        std::cout << "Encoder::add() string()->" << this->buf.subslice(0, this->pos).to_string() << std::endl;

        return this;
    }

    Slice<char> Encoder::next(int n)
    {
        auto p = this->pos + n;
        if ( this->buf.size() < p) {
            this->grow(p);
        }
        
        auto data = this->buf.subslice(this->pos, n);
        this->pos = p;
        return data;
    }

    void Encoder::grow(int p) 
    {
        auto s = this->buf.size() << 1;
        int len = 0;
        if (p < 1024) {
            len = 1024;
        } 
        else if ( p < s) {
            len = s;
        }
        
        char* addr = new(std::nothrow) char[len];

        Array<char> b(addr, len);
        if (this->pos > 0) {
            auto data = this->buf.subslice(0, this->pos);
            b.copy( data);
        }
        this->buf = std::move(b);
    }

    // Bytes returns encoded bytes of the last session description.
    // The bytes stop being valid at the next encoder call.
    Slice<char> Encoder::Bytes()
    {
        if (this->newline) {
            auto b = this->next(2);
            b[0], b[1] = '\r', '\n';
            this->newline = false;
        }
        return this->buf.subslice(0, this->pos);
    }

    // Bytes returns the encoded session description as str.
    string Encoder:: String()  
    {
        return this->Bytes().to_string();
    }
