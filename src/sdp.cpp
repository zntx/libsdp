#include <utility>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include "sdp.h"
#include "encoder.h"

using namespace std;


    // String returns the encoded session description as string.
    string Session::String()
    {
        return this->Bytes().to_string();
    }

    //Bytes returns the encoded session description as buffer.
    Slice<char>  Session::Bytes()
    {
        auto encoder = new(std::nothrow) Encoder{{nullptr, 0}, 0,0};
        //return encoder->session(this)->Bytes();
		auto slice =  encoder->session(this)->Bytes();
		
		std::cout << "Session::Bytes() slice.size() = " << slice.size() << std::endl;
		std::cout << "Session::Bytes() slice.to_string() = " << slice.to_string() << std::endl;
			
		return slice ;
    }


    // Format returns format description by payload type.
	Format* Media::find_Format( int pt )
    {
		for (auto &&f : this->formats ){
			if (f->Payload == pt) {
				return f;
			}
		}
		return nullptr;
	}

    // NegotiateMode negotiates streaming mode.
	string NegotiateMode(string local, string remote )  
    {
		if( local == SendRecv){
			if( remote == RecvOnly)
				return SendOnly;
			else if( remote ==  SendOnly)
				return RecvOnly;
			else
				return remote;
		}
		else if( local == SendOnly ){
			if( remote == SendRecv || remote == RecvOnly)
				return SendOnly;
		}
		else if( local == RecvOnly ){
			if( remote ==  SendRecv || remote == SendOnly)
				return RecvOnly;
		}

		return Inactive;
	}

	// DeleteAttr removes all elements with name from attrs.
	// Attributes DeleteAttr(Attributes attrs , string name ... )  
    // {
	// 	auto n = 0;
	// loop:
	// 	for _, it := range attrs {
	// 		for _, v := range name {
	// 			if it.Name == v {
	// 				continue loop
	// 			}
	// 		}
	// 		attrs[n] = it
	// 		n++
	// 	}
	// 	return attrs[:n]
	// }
    
