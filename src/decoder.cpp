
#include <utility>
#include <chrono>
#include <string>
#include <algorithm>
#include "sdp.h"
#include "decoder.h"

namespace sdp{
    using namespace std;


    #define maxLineSize 1024

    #define errLineTooLong     ErrDecode::New(string("sdp: line is too long"))
    //#define errUnexpectedField ErrDecode::New(string("unexpected field"))
    //#define errFormat          ErrDecode::New(string("format Error"))

    ErrDecode errUnexpectedField{ 0, 0, string("unexpected field")};
    ErrDecode errFormat{ 0, 0, string("format Error")};
    ErrDecode errCode{ 0, 0, string()};


    string ErrDecode::to_string() const
	{
        return string("sdp: ") + std::to_string(err)  + string(" on line") + std::to_string(this->line) +  this->text;
    }

//	ErrDecode* ErrDecode::New( string _text )
//	{
//		return new ErrDecode{ 1, 1, std::move(_text)};
//	}

	StringReader::StringReader( string _s) : s(std::move(_s)) {

	}

    pair<string, Error> StringReader::ReadLine() 
	{
        auto s = this->s;
        auto n = this->s.size();
        if( n == 0 ){
            errCode.err = EOF;
            errCode.line = 0;
            errCode.text =  "empty";
            return make_pair(string(""), &errCode);
        }
        auto i = 0;
        for( const char& ch : s ){
            if (ch == '\n' ){
                this->s = s.substr(i+1);
                while (i > 0 && s[i-1] == '\r' ){
                    i--;
                }
                return make_pair(s.substr(0,i), nullptr);
            }
            i++;
        }
        this->s = "";
        return make_pair(s, nullptr);
    }


// type reader struct {
// 	b *bufio.Reader
// }

// func (r *reader) ReadLine() (string, Error) {
// 	b, prefix, err := r.b.ReadLine()
// 	if prefix && err == nullptr {
// 		err = errLineTooLong
// 	}
// 	if err != nullptr {
// 		return "", err
// 	}
// 	return string(b), nullptr
// }



	Decoder::~Decoder(){
		delete r;
	}
	// Decode encodes the session description.
	pair<Session*, Error> Decoder::tryDecode() 
	{
		auto line = 0;
		auto sess = new Session;
		Media* media = nullptr;

		while(true) {
			line++;
			auto s_err = this->r->ReadLine();
			auto s = s_err.first;
			auto err = s_err.second;
			if( err != nullptr ) {
				if( err->err == EOF && sess->origin != nullptr) {
					break;
				}
				return make_pair(nullptr, err);
			}

			if (s.size() == 0 && sess->origin != nullptr) {
				break;
			}
			if (s.size() < 2 || s[1] != '=' ){
                errCode.err = 3;
                errCode.line = line;
                errCode.text = s;
				return make_pair(nullptr, &errCode);
			}

			auto f = s[0];
			auto v = s.substr(2);

			// std::cout << line << " " << f << std::endl;
			// std::cout << line << " " << v << std::endl;

			if( f == 'm' ){
				media = new(std::nothrow) Media;
				err = this->media(media, f, v);
				if( err == nullptr ){
					sess->media.push_back(media);
				}
			} 
			else if (media == nullptr ){
				err = this->session(sess, f, v);
			} 
			else {
				err = this->media(media, f, v);
			}
			
			if (err != nullptr ){
				std::cout << line << " " << err->to_string() << std::endl;
				std::cout << line << " "   << std::endl;
				return make_pair(nullptr,  err );
			}
		}
		return make_pair(sess, nullptr);
	}

	Error Decoder::session( Session* s,  char f,  const string& v)
	{
		Error err = nullptr ;
		
		if( f == 'v'){
			auto version_err = this->int64(v);
			s->Version = version_err.first;
			       err = version_err.second;
		}
		else if( f == 'o'){
			if (s->origin != nullptr ){
				return &errUnexpectedField;
			}
			auto origin_err = this->origin(v);
			s->origin = origin_err.first;
			err       = origin_err.second;
		}
		else if( f == 's'){
			s->Name = v;
		}
		else if( f == 'i'){
			s->Information = v;
		}
		else if( f == 'u'){
			s->URI = v;
		}
		else if( f == 'e'){
			s->Email.push_back( v);
		}
		else if( f == 'p'){
			s->Phone.push_back( v);
		}
		else if( f == 'c'){
			if (s->connection != nullptr) {
				std::cout << "err" << " "   << std::endl;
				return &errUnexpectedField;
			}
			auto connection_err = this->connection(v);
			s->connection = connection_err.first;
			err = connection_err.second;
		}
		else if( f == 'b'){
			// if (s->bandwidth == nullptr) {
			// 	s->bandwidth = new(nothrow) Bandwidth;
			// }
			err = this->bandwidth(s->bandwidth, v);
		}
		else if( f == 'z'){
			auto timeZone_err = this->timezone(v);
			s->timeZone =  timeZone_err.first;
			err = timeZone_err.second;
		}
		else if( f == 'k'){
			s->key.push_back( this->key(v));
		}
		else if( f == 'a'){
			auto a = this->attr(v);
			if(a->Name == ModeInactive ||a->Name ==  ModeRecvOnly || a->Name ==  ModeSendOnly || a->Name ==  ModeSendRecv)
				s->Mode = a->Name;
			else
				s->attributes.attr.push_back ( a);
		}
		else if( f == 't'){
			auto timing_err = this->timing(v);
			s->timing = timing_err.first;
			      err = timing_err.second;
		}
		else if( f == 'r'){
			auto r_err = this->repeat(v);
			auto r = r_err.first;
			err = r_err.second;
			if (err != nullptr ){
				return err;
			}
			s->repeat.push_back (  r);
		}
		else {

			return &errUnexpectedField;
		}

		return err;
	}

	Error Decoder::media(Media* m, char f, const string& v)
	{
		Error err = nullptr;
		 
		if( f == 'm')
			err = this->proto(m, v);
		else if( f == 'i')
			m->Information = v;	
		else if( f == 'c') {
			auto conn_err = this->connection(v);
			auto conn = conn_err.first;
			err = conn_err.second; 
			if (err != nullptr ){
				return err;
			}
			m->connection.push_back(  conn);
		}
		else if( f == 'b') {
			// if (m->bandwidth == nullptr) {
			// 	m->bandwidth = new(nothrow) Bandwidth;
			// }
			err = this->bandwidth(m->bandwidth, v);
		}
		else if( f ==  'k')
			m->key.push_back (  this->key(v));
		
		else if( f == 'a') {
			auto a = this->attr(v);
			 
			if( a->Name == ModeInactive || a->Name == ModeRecvOnly  || a->Name == ModeSendOnly  || a->Name == ModeSendRecv)
				m->Mode = a->Name;
			else if( a->Name =="rtpmap" || a->Name == "rtcp-fb" || a->Name ==  "fmtp")
				err = this->format(m, a);
			else
				m->attributes.attr.push_back (  a);
		}
		else { 
			return &errUnexpectedField;
		}
		return err;
	}

	Error Decoder::format(Media* m, Attr* a)
	{
		auto p_ok = this->fields(a->Value, 2);
		auto p = p_ok.first;
		auto ok = p_ok.second;
		if (!ok) {
			return &errFormat;
		}

		auto pt_err = this->int64(p[0]);
		auto pt = pt_err.first;
		auto err = pt_err.second;
		if( err != nullptr) {
			return err;
		}

		auto f = m->find_Format(pt);
		auto v =  p[1];
		if (f == nullptr) {
			return nullptr;
		}
		 
		if( a->Name == "rtpmap")
			err = this->rtpmap(f, v);
		else if( a->Name == "rtcp-fb")
			f->Feedback.push_back (  v);
		if( a->Name == "fmtp" )
			f->Params.push_back (  v);
		
		return err;
	}

	Error Decoder::rtpmap(Format* f,  const string& v)
	{
		auto p_ok = this->split(v, '/', 3);
		auto p = p_ok.first;
		auto ok = p_ok.second;
		if(p.size() < 2) {
			return &errFormat;
		}

		f->Name = p[0];
		Error err ;
		if (ok) {
			auto channels_err = this->int64(p[2]);
			f->Channels = channels_err.first;
				    err = channels_err.second;
			if( err != nullptr ){
				return err;
			}
		}

		auto clockRate_err = this->int64(p[1]); 
		f->ClockRate = clockRate_err.first; 
		         err = clockRate_err.second; 
		if (err != nullptr ){
			return err;
		}
		return nullptr;
	}

	Error Decoder::proto(Media* m, const string& v)
	{
		auto p_ok = this->fields(v, 4);
		auto p = p_ok.first;
		auto ok = p_ok.second;
		if (!ok ){
			return &errFormat;
		}

		auto formats = p[3];
		m->Type = p[0] ;
        m->Proto = p[2];
		auto p_ok2 = this->split(p[1], '/', 2);
		p = p_ok2.first;
		ok= p_ok2.second;
		Error err ;
		if( ok ){
			auto num_err = this->int64(p[1]);
			m->PortNum = num_err.first;
			err =num_err.second;
			if (err != nullptr) {
				return err;
			}
		}

		auto port_err = this->int64(p[0]);
		m->Port = port_err.first;
		    err = port_err.second;
		if ( err != nullptr ){
			return err;
		}

		auto p__ = this->fields(formats, maxLineSize);
		p = p__.first;
		for ( auto && it : p) {
			if( it == "*") {
				continue;
			}
			auto pt_err = this->int64(it);
			auto pt = pt_err.first;
			err = pt_err.second;
			if (err != nullptr) {
				return err;
			}
			m->formats.push_back( new Format{pt});
		}
		return nullptr;
	}

	pair<Origin*, Error> Decoder::origin(const string& v)
	{
		auto p_ok = this->fields(v, 6);
		auto p = p_ok.first;
		auto ok = p_ok.second;
		if( !ok ){
			return make_pair(nullptr, &errFormat);
		}
		auto o = new(nothrow) Origin;
        o->Username = p[0];
        o->Network = p[3];
        o->Type = p[4];
        o->Address = p[5];
		Error err ;

		auto id_err = this->int64(p[1]);
		o->SessionID = id_err.first;
		         err = id_err.second;
		if ( err != nullptr ){
			return make_pair(nullptr, err);
		}

		auto ersion_err = this->int64(p[2]);
		o->SessionVersion = ersion_err.first;
		err = ersion_err.second;
		if ( err != nullptr ){
			return make_pair(nullptr, err);
		}
		return make_pair(o, nullptr);
	}

	pair<Connection*, Error> Decoder::connection(const string& v)
	{
		auto p_ok = this->fields(v, 3);
		auto p = p_ok.first;
		auto ok = p_ok.second;
		if( !ok ){
			return make_pair(nullptr, &errFormat);
		}

		auto c = new(nothrow) Connection;
		c->Network = p[0];
		c->Type = p[1];
		c->Address = p[2];
		p_ok = this->split(c->Address, '/', 3);
		p = p_ok.first;
		ok = p_ok.second;
		if( ok ){
			auto ttl_err = this->int64(p[1]);
			auto ttl = ttl_err.first;
			auto err = ttl_err.second;
			if (err != nullptr ){
				return make_pair(nullptr, err);
			}
			c->TTL = ttl;
			//p = p[1:];
			p.erase(p.begin());
		}

		if ( p.size() > 1 ){
			auto num_err = this->int64(p[1]);
			auto num = num_err.first;
			auto err = num_err.second;
			if (err != nullptr ){
				return make_pair(nullptr, err);
			}
			c->Address = p[0] ;
			c->AddressNum = num;
		}

		return make_pair(c, nullptr);
	}

	Error Decoder::bandwidth(Bandwidth& b, const string& v)
	{
		auto p_ok = this->split(v, ':', 2);
		auto p = p_ok.first;
		auto ok = p_ok.second;
		if (!ok ){
			return &errFormat;
		}

		auto val_err = this->int64(p[1]);
		auto val = val_err.first;
		auto err = val_err.second;
		if (err != nullptr ){
			return err;
		}
		b[p[0]] = val;
		return nullptr;
	}

	pair<vector<TimeZone*>, Error> Decoder::timezone(const string& v)
	{
		auto p_ok = this->fields(v, 40);
		auto p = p_ok.first;
		auto ok = p_ok.second;

		vector<TimeZone*> zone ;
		Error err ;
		while( p.size() > 1 ){
			auto it = new(nothrow) TimeZone;

			auto time_err = this->time(p[0]);
			it->Time = time_err.first;
			err = time_err.second;
			if ( err != nullptr) {
				return make_pair(vector<TimeZone*>{}, err);
			}

			auto Offset_err = this->duration(p[1]);
			it->Offset  = Offset_err.first;
			err = Offset_err.second;
			if ( err != nullptr) {
				return make_pair(vector<TimeZone*>{}, err);
			}
			zone.push_back( it);
			//p = p[2:];
			p.erase(p.begin());
			p.erase(p.begin());
		}
		return make_pair(zone, nullptr);
	}

	Key* Decoder::key(const string& v)
	{
		auto p_ok = this->split(v, ':', 2); 
		auto p = p_ok.first;
		auto ok = p_ok.second;
		if( ok ){
			return new Key{p[0], p[1]};
		}
		return new Key{v, ""};
	}

	Attr* Decoder::attr( const string& v )
	{
		auto p_ok = this->split(v, ':', 2);
		auto p = p_ok.first;
		if( p_ok.second ){
			return new Attr{p[0], p[1]};
		}
		return new Attr{v, ""};
	}

	pair<Timing*, Error> Decoder::timing(const string& v)
	{
		auto p_ok = this->fields(v, 2);
		auto p = p_ok.first;
		auto ok = p_ok.second;
		if( !ok ){
			return make_pair(nullptr, &errFormat);
		}

		std::cout << "err 1111" << p[0]   << std::endl;
		std::cout << "err 1111" << p[1]    << std::endl;
		
		auto start_err = this->time(p[0]);
		auto start = start_err.first;
		auto err = start_err.second;
		if( err != nullptr ){
			return make_pair(nullptr, err);
		}

		auto stop_err = this->time(p[1]);
		auto stop = stop_err.first;
		err =  stop_err.second;
		if (err != nullptr) {
			return make_pair(nullptr, err);
		}
		return make_pair( new Timing{start, stop}, nullptr);
	}

	pair<Repeat*, Error> Decoder::repeat(const string& v)
	{
		auto p_ = this->fields(v, maxLineSize);
		auto p = p_.first;
		if ( p.size()< 2) {
			return make_pair(nullptr, &errFormat);
		}
		auto r = new(std::nothrow) Repeat;
		Error err = nullptr;
		
		auto interval_err = this->duration(p[0]);
		r->Interval = interval_err.first;
		        err = interval_err.second;
		if(  err != nullptr) {
			return make_pair(nullptr, err);
		}

		auto duration_err = this->duration(p[1]);
		r->Duration = duration_err.first;
		        err = duration_err.second;
		if(  err != nullptr) {
			return make_pair(nullptr, err);
		}

		for(auto i = 2 ; i< p.size(); i++) {
			auto off_err = this->duration(p[i]);
			auto off = off_err.first;
                                  err = off_err.second;
			if (err != nullptr ){
				return make_pair(nullptr, err);
			}
			r->Offsets.push_back( off);
		}
		return make_pair( r, nullptr);
	}

	pair<chrono::time_point<chrono::system_clock>, Error> Decoder::time(string v) 
	{
		auto sec_err = this->int64(std::move(v));
		auto sec = sec_err.first;
		auto err = sec_err.second;
		if (err != nullptr || sec == 0 ){
			return make_pair(chrono::system_clock::now(), err);
		}


		return make_pair(chrono::system_clock::now() + chrono::seconds(sec), nullptr);
	}

	pair<chrono::seconds, Error> Decoder::duration(string& v) 
	{
		auto m =  1;
		auto n = v.size() - 1;
		if ( !v.empty() ){
			switch (v[n]) {
			case 'd':
				m = 86400;
				v = v.substr(0, n);
				break;
			case 'h':
				m = 3600;
				v =  v.substr(0, n);
				break;
			case 'm':
				m = 60;
				v = v.substr(0, n);
				break;
			case 's':
				v = v.substr(0, n);
				break;
			default:
				;
			}
		}
		auto sec_err = this->int64(v);
		auto sec = sec_err.first;
		auto err = sec_err.second;
		if (err != nullptr ){
			return make_pair(chrono::seconds(0), err);
		}
		return make_pair(chrono::seconds( sec * m ), nullptr);
	}

	pair<size_t, Error> Decoder::int64(const string& v)
	{
		// if ( !v.empty() && std::all_of(v.begin(), v.end(), ::isdigit))
		// 	return make_pair( std::stoull(v), nullptr);
		// else
		// 	return make_pair( 0, ErrDecode::New("is not number"));

		try {
			int64_t val = std::stoul(v);
			return make_pair( val, nullptr);;
		} 
		catch (const std::invalid_argument&) {

            errCode.text = "is not number";
			return make_pair( 0, &errCode);
		} 
		catch (const std::out_of_range&) {

            errCode.text = "is not number";
			return make_pair( 0, &errCode);
		}
	}

	pair<vector<string>, bool> Decoder::fields(const string& s, int n)
	{
		return this->split(s, ' ', n);
	}

	pair<vector<string>, bool> Decoder::split(const string& s, char sep, int n )
	{
		//vector<string> p(this->p);
		vector<string> p;
		auto pos = 0;
		auto i = -1;
		for( auto c :  s) {
			i++;
			if (c != sep) {
				continue;
			}

			auto len = i - pos;
			if (len <= 0) len = 1;
		
			p.push_back( s.substr(pos, len));
			pos = i + 1;
			if (p.size() >= n-1) {
				break;
			}
		}
		p.push_back(s.substr(pos));
		//this->p = p;
		return make_pair(p, p.size() == n);
	}



    // NewDecoder returns new decoder that reads from r.
    // Decoder* NewDecoder(r io.Reader) *Decoder {
    // 	return &Decoder{r: &reader{b: bufio.NewReaderSize(r, maxLineSize)}}
    // }

    // NewDecoderString returns new decoder that reads from s.
    Decoder* NewDecoderString(string s) {
        return new Decoder{ new(nothrow) StringReader(std::move(s))};
    }

    // ParseString reads session description from the string.
    pair<Session*, Error>  ParseString( string s) {
        Decoder decode = { new StringReader(std::move(s))};
        return decode.tryDecode();

    }

    // Parse reads session description from the buffer.
    pair<Session*, Error> Parse( char* b) {
        return ParseString(string(b));
    }
}