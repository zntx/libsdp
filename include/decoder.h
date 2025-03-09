#ifndef DECODER_H_H
#define DECODER_H_H


#include <utility>
#include <chrono>
#include <string>
#include <errno.h>
#include <algorithm>
#include "sdp.h"

using namespace std;


struct ErrDecode  {
	int err ;
	int line; 
	string text; 

    string to_string();

	static ErrDecode* New( string _text );
};

typedef  ErrDecode* Error ;


class lineReader {
public:
	virtual pair<string, Error> ReadLine() = 0;
};

class StringReader : public lineReader {
	string s ;

public:
	StringReader( string _s);

    pair<string, Error> ReadLine() override;
};

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


// A Decoder reads a session description from a stream.
struct Decoder  {
	lineReader* r; 
	vector<string> p; 

	~Decoder();

	// Decode encodes the session description.
	pair<Session*, Error> tryDecode() ;
    Error session( Session* s,  char f,  string v) ;

    
    Error media(Media* m, char f, string v);
    
    Error format(Media* m, Attr* a);

    Error rtpmap(Format* f,  string v);
    
    Error proto(Media* m, string v) ;
    
    pair<Origin*, Error> origin(string v) ;
    
    pair<Connection*, Error> connection(string v) ;
    
    Error bandwidth(Bandwidth& b, string v);

    pair<vector<TimeZone*>, Error> timezone(string v);

    Key* key(string v);

    Attr* attr( string v ) ;

    pair<Timing*, Error> timing(string v) ;

    pair<Repeat*, Error> repeat(string v) ;

    pair<chrono::time_point<chrono::system_clock>, Error> time(string v);

    pair<chrono::seconds, Error> duration(string& v);
    
    pair<int64_t, Error> int64(string v);

    pair<vector<string>, bool> fields(string s, int n);
    
    pair<vector<string>, bool> split(string s, char sep, int n );
 
};



// NewDecoder returns new decoder that reads from r.
// Decoder* NewDecoder(r io.Reader) *Decoder {
// 	return &Decoder{r: &reader{b: bufio.NewReaderSize(r, maxLineSize)}}
// }

// NewDecoderString returns new decoder that reads from s.
Decoder* NewDecoderString(string s) ;

// ParseString reads session description from the string.
pair<Session*, Error>  ParseString( string s) ;

// Parse reads session description from the buffer.
pair<Session*, Error> Parse( char* b);

#endif