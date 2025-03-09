#ifndef SDP_H_H
#define SDP_H_H


#include <utility>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include "attribute.h"
#include "Slice.h"

namespace sdp{  
    using namespace std;

    // ContentType is the media type for an SDP session description.
    #define ContentType  "application/sdp"

    // Streaming modes.
    #define	SendRecv  "sendrecv"
    #define	SendOnly  "sendonly"
    #define	RecvOnly  "recvonly"
    #define	Inactive  "inactive"

    // Origin represents an originator of the session.
    struct Origin  {
        string  Username;       
        size_t SessionID;
        size_t SessionVersion;
        string  Network ;       
        string  Type;           
        string  Address;        
    };

    // Connection contains connection data.
    struct Connection  {
        string 	Network;    
        string 	Type  ;     
        string 	Address ;
        size_t 	TTL = 0 ;
        size_t 	AddressNum = 0;
    };


    // Bandwidth contains session or media bandwidth information.
    typedef  map<string, size_t> Bandwidth;

    // TimeZone represents a time zones change information for a repeated session.
    struct TimeZone  {
        // 使用 std::chrono::steady_clock 定义时间点成员变量
        chrono::time_point<chrono::system_clock>  Time;
        chrono::seconds Offset;
    };

    // Key contains a key exchange information.
    // Deprecated: Not recommended, supported for compatibility with older implementations.
    struct Key  {
        string Method;
		string Value; 
    };

    // Timing specifies start and stop times for a session.
    struct Timing  {
        chrono::time_point<chrono::system_clock> Start;
        chrono::time_point<chrono::system_clock> Stop;
    };

    // Repeat specifies repeat times for a session.
    struct Repeat  {
        chrono::seconds Interval ;
        chrono::seconds Duration;
        vector<chrono::seconds> Offsets  ;
    };

    // Format is a media format description represented by "rtpmap" attributes.
    struct Format  {
        size_t 		Payload ;
        string 	    Name ;
        size_t 		ClockRate ;
        size_t 		Channels ;
        vector<string>  Feedback;  // "rtcp-fb" attributes
        vector<string>  Params;    // "fmtp" attributes
    };


    // Media contains media description.
    struct Media  {
        string 			Type ;    
        size_t 			Port ;
        size_t 			PortNum ;
        string 			Proto ; 

        string 			Information; // Media Information ("i=")
        vector<Connection*> 	connection ;  // Connection Data ("c=")
        Bandwidth 		bandwidth;  // Bandwidth ("b=")
        vector<Key*> 	key ;       // Encryption Keys ("k=")
        Attributes      attributes; // Attributes ("a=")

        string			Mode ;   // Streaming mode ("sendrecv", "recvonly", "sendonly", or "inactive")
        vector<Format*>	formats; // Media Formats ("rtpmap")

        // Format returns format description by payload type.
        Format* find_Format(size_t pt );
    };

    // Session represents an SDP session description.
    struct Session  {
        size_t            Version = 0;  // Protocol Version ("v=")
        Origin*           origin = nullptr;      // Origin ("o=")
        string            Name;        // Session Name ("s=")
        string            Information; // Session Information ("i=")
        string            URI;         // URI ("u=")
        vector<string>    Email;       // Email Address ("e=")
        vector<string>    Phone;       // Phone Number ("p=")
        Connection*       connection  = nullptr;  // Connection Data ("c=")
        Bandwidth         bandwidth;   // Bandwidth ("b=")
        vector<TimeZone*> timeZone;    // TimeZone ("z=")
        vector<Key*>      key;         // Encryption Keys ("k=")
        Timing*           timing  = nullptr;      // Timing ("t=")
        vector<Repeat*>   repeat;      // Repeat Times ("r=")
        Attributes        attributes;  // Session Attributes ("a=")
        vector<Media*>    media;    // Media Descriptions ("m=")

        string            Mode; // Streaming mode ("sendrecv", "recvonly", "sendonly", or "inactive")

        // String returns the encoded session description as string.
        string String() ;

        // Bytes returns the encoded session description as buffer.
        Slice<char>  Bytes();
    };

    // NegotiateMode negotiates streaming mode.
	string NegotiateMode(const string& local, string remote );
}
#endif
