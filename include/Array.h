#ifndef ARRAY_H
#define ARRAY_H

#include "Slice.h"


template <typename T>
class Array : public Slice<T> {
public:

    ~Array() {
        if( this->addr != nullptr)
            delete this->addr;
    }

    // Array( std::size_t size) {
    //     this->addr = new(std::nothrow) char[size];
    //     if(this->addr == nullptr)
    //         this->len = 0;
    //     else
    //         this->len = size;
    // }

    Array(T* address, std::size_t size) :Slice<T>(address, size) {
 
    }

    Array( Array<T> &other) = delete;

    Array(const Array<T> &other) = delete;

    // 移动构造
    Array(Array<T> &&other)  noexcept {
        //printf("Slice1 移动构造\n");
        if( this->addr != nullptr)
            delete this->addr;

        this->addr = other.addr;
        this->len  = other.len;

        other.addr = nullptr;
        other.len = 0;
    }

    //复制赋值运算符
    Array& operator=(Array<T> &other) = delete;

    // 移动赋值运算符
    // 和复制赋值运算符的区别在于，其参数是右值引用
    Array& operator= (Array<T> &&other)  noexcept {
        //printf("Slice1 移动赋值运算符\n");
        if( this->addr != nullptr)
            delete this->addr;

        this->addr = other.addr;
        this->len  = other.len;

        other.addr = nullptr;
        other.len = 0;

        return *this;
    }


};

#endif //ARRAY_H