//
// Created by zz on 2025/3/2.
//

#ifndef SDP_SLICE_H
#define SDP_SLICE_H

//#include <atomic>
//#include <stddef.h>
//#include <stddef.h>

#include <iostream>
#include <list>
#include <vector>

template <typename T>
class Slice {
public:
    T* addr;
    std::size_t len{};

public:
    Slice(T* address, std::size_t size) {
        addr = address;
        len = size;
    }

    Slice( Slice<T> &other) {
        this->addr = other.addr;
        this->len  = other.len;
    }

    Slice(const Slice<T> &other) {
        this->addr = other.addr;
        this->len  = other.len;
    }
    // 移动构造
    Slice(Slice<T> &&other)  noexcept {
        this->addr = other.addr;
        this->len  = other.len;
    }

    //复制赋值运算符
    Slice& operator= (Slice<T> &other) {
        this->addr = other.addr;
        this->len  = other.len;

        return *this;
    }

    // 移动赋值运算符
    // 和复制赋值运算符的区别在于，其参数是右值引用
    Slice& operator= (Slice<T> &&other)  noexcept {
        this->addr = other.addr;
        this->len  = other.len;

        return *this;
    }

    bool operator==( Slice<T> other) {
        if(this->len != other.size())
            return false;

        for( size_t index = 0; index < other.size(); index++ )
            if(this->at(index) != other.at(index) )
                return false;

        return true;
    }


    bool is_legal() {
        return this->addr != nullptr && this->len > 0;
    }

    bool no_legal() {
        return this->addr == nullptr || this->len <= 0;
    }

    // 使用 SFINAE 技术，只有当 T = char 时才有 split 函数
    template <typename U = T,
            typename std::enable_if<std::is_same<U, char>::value, int>::type = 0>
    bool is_digit( ) {
        for( std::size_t index = 0; index < this->len; index++) {
            if( *(this->addr + index) < '0' || *(this->addr + index) > '9')
                return false;
        }
        return true;
    }

    T& operator[](std::size_t i) {
        if( i > this->len)
            throw "out of array";

        return this->addr[i];
    }

    std::size_t size(){
        return this->len;
    }

    bool at(size_t index, T &ch) {
        if (index >= this->len)
            return false;

        ch = *(this->addr + index);
        return true;
    }

    /*   */
    T at(size_t index) {
        return *(this->addr + index);
    }

    bool set(size_t index, T ch) {
        if( index <= this->len ) {
            *( this->addr + index) = ch;
            return true;
        }

        return false;
    }

    std::size_t find( T ch, size_t start = 0) {
        for( std::size_t index = start; index < this->len; index++) {
            if( *( this->addr + index) == ch)
                return index;
        }
        return this->len;
    }

    std::size_t find_count( T ch, size_t start = 0)  {
        std::size_t count = start;
        for( std::size_t index = start; index < this->len; index++)
        {
            if( *( this->addr + index) == ch)
                count++;
        }
        return count;
    }

    std::size_t find( const T* ch, size_t ch_len, size_t start = 0) {
        std::size_t j = 0;
        for( std::size_t i = start; i < this->len - ch_len; i++) {
            for(  j = 0; i < ch_len; j++) {
                if( *( this->addr + i + j) != *( ch + j))
                    break;
            }

            if( j == ch_len)
                return i;
        }
        return this->len;
    }

    std::size_t find( Slice<T> ch, size_t start = 0) {
        std::size_t j = 0;
        for( std::size_t i = start; i < this->len - ch.len; i++) {
            for(  j = 0; i < ch.len; j++) {
                if( *( this->addr + i + j) != *( ch.addr + j))
                    break;
            }

            if( j == ch.len)
                return i;
        }
        return this->len;
    }

    Slice<T> subslice(size_t start, size_t _len = 0)
    {
        if ( start >= this->len)
            return {nullptr, 0};

        if(_len == 0 )
            return {this->addr + start, this->len - start};

        if (start + _len >= this->len)
            return {nullptr, 0};

        return {this->addr + start, _len};
    }

    std::string to_string()
    {
        return std::string((char*)this->addr, this->len);
    }


    std::size_t copy(Slice& other) {
        auto i = 0;
        for( i = 0; i< this->len && i< other.len; i++) {
            *(this->addr + i ) = *(other.addr + i);
        }

        return i;
    }

    std::size_t copy(const T* _addr, size_t _len) {
        auto i = 0;
        for( i = 0; i< this->len && i< _len; i++) {
            *(this->addr + i ) = *(_addr + i);
        }

        return i;
    }

    std::size_t append(Slice& other) {
        auto i = 0;
        for( i = 0; i< this->len && i< other.len; i++) {
            *(this->addr + i ) = *(other.addr + i);
        }

        return i;
    }

    std::list<Slice<T>> split(T sep, int number = 0) {
        std::list<Slice<T>> array;
        auto i = 0;
        auto pos  = 0;
        for( i = 0; i< this->len; i++) {
            if (*(this->addr + i ) != sep ){
                continue;
            }

            if(  i - pos > 0)
                array.push_back( this->subslice(pos, i - pos));
            //p.append( s[pos:i]);
            pos = i + 1;
            if ( sep > 0 && array.size() >= number - 1 ){
                break;
            }
        }

        if( pos < this->len )
            array.push_back( this->subslice(pos, this->len - pos));

        return array;
    }

    // 使用字符串分割
    std::list<Slice<T>> split(Slice<T> sep, int number = 0) {
        std::list<Slice<T>> array;
        auto i = 0;
        auto pos  = 0;
        for( i = 0; i< this->len - sep.len; i++) {
            if( this->subslice(i, sep.size() ) != sep)
                continue;

            if(  i - pos > 0)
                array.push_back( this->subslice(pos, i - pos));
            //p.append( s[pos:i]);
            pos = i + sep.len;
            if ( sep > 0 && array.size() >= number - 1 ){
                break;
            }
        }

        if( pos < this->len - sep.len)
            array.push_back( this->subslice(pos, this->len - pos));

        return array;
    }



    // 使用 SFINAE 技术，只有当 T = char 时才有 split 函数
    template <typename U = T,
            typename std::enable_if<std::is_same<U, char>::value, int>::type = 0>
    int atoi( ) {
        int sum = 0;
        for( std::size_t index = 0; index < this->len; index++) {
            if( *(this->addr + index) < '0' || *(this->addr + index) > '9')
                return 0;

            sum = sum * 10 + *(this->addr + index) - '0';
        }
        return sum;
    }

};



#endif //SDP_SLICE_H
