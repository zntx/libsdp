# go-sdp

C++11 implementation of SDP (Session Description Protocol). No external dependencies.</br>
which is a re - implementation of go-sdp

[![Build Status](https://api.travis-ci.org/pixelbender/go-sdp.svg)](https://travis-ci.org/pixelbender/go-sdp)
[![Coverage Status](https://coveralls.io/repos/github/pixelbender/go-sdp/badge.svg?branch=master)](https://coveralls.io/github/pixelbender/go-sdp?branch=master)
[![Go Report Card](https://goreportcard.com/badge/github.com/pixelbender/go-sdp)](https://goreportcard.com/report/github.com/pixelbender/go-sdp)
[![GoDoc](https://godoc.org/github.com/pixelbender/go-sdp?status.svg)](https://godoc.org/github.com/pixelbender/go-sdp/sdp)

## Features

- [x] SDP Encoder/Decoder

## Installation

```sh
go get github.com/pixelbender/go-sdp/sdp
```

## SDP Decoding

```c
int main() {
	auto sess = sdp::ParseString(R"(v=0
o=alice 2890844526 2890844526 IN IP4 alice.example.org
s=Example
c=IN IP4 127.0.0.1
t=0 0
a=sendrecv
m=audio 10000 RTP/AVP 0 8
a=rtpmap:0 PCMU/8000
a=rtpmap:8 PCMA/8000)")

	if(sess.second == nullptr){
		std::cout << "  " << sess.second->to_string() << std::endl;
	} else {
		// prints PCMU
	    std::cout << "  " << sess.first->media[0]->formats[0]->Name << std::endl;
		delete sess.first;
	}
}
```

## SDP Encoding

```go


func main() {
	auto sess := &sdp::Session{
    		Origin: &sdp.Origin{
    			Username:       "alice",
    			Address:        "alice.example.org",
    			SessionID:      2890844526,
    			SessionVersion: 2890844526,
    		},
    		Name:       "Example",
    		Connection: &sdp.Connection{
    			Address: "127.0.0.1",
            },
    		Media: []*sdp.Media{
    			{
    				Type:  "audio",
    				Port:  10000,
    				Proto: "RTP/AVP",
    				Formats: []*sdp.Format{
    					{Payload: 0, Name: "PCMU", ClockRate: 8000},
    					{Payload: 8, Name: "PCMA", ClockRate: 8000},
    				},
    			},
    		},
    		Mode: sdp.ModeSendRecv,
    	}
    	
	fmt.Println(sess.String())
}
```

## Attributes mapping

| Scope | Attribute | Property |
| ----- | --------- | ----------------- |
| session, media | sendrecv, recvonly, sendonly, inactive | Session.Mode, Media.Mode |
| media | rtpmap | Media.Format |
| media | rtcp-fb | Format.Feedback |
| media | fmtp | Format.Params |

## Specifications

- [RFC 4566: Session Description Protocol](https://tools.ietf.org/html/rfc4566)
