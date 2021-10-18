
#pragma once


#include <avr/pgmspace.h>
#include <stdint.h>



template<typename Type>
struct remove_const_t {
	using value = Type;	
};
template<typename Type>
struct remove_const_t<const Type> {
	using value = Type;	
};




template<typename Type>
class ProgmemRef {
public:
    using type = Type;

    constexpr ProgmemRef(const Type& ref): ptr(&ref) {}
    
    Type operator()() const {
        return read();
    }
    
    operator Type() const {
        return read();
    }
    
    Type _read_block() const {
		union {
			char _;
			typename remove_const_t<Type>::value obj;
		} _uobj;
        memcpy_P(&_uobj.obj, ptr, sizeof(Type));
        return _uobj.obj;
    }

    Type read() const {
       return  _read_block();
    }

    constexpr const Type* get_ptr() const {
      return ptr;
    }


private:
    const Type* ptr;

};


template<size_t _size, typename Type>
class ProgmemRef<Type[_size]> {
public:
    using type = Type;

    constexpr ProgmemRef(const Type* const ref): ptr(ref), size(_size) {}

    bool operator==(ProgmemRef ref) const noexcept {
        return ref.ptr == ptr;
    }
    bool operator!=(ProgmemRef ref) const noexcept {
        return ref.ptr != ptr;
    }

    constexpr ProgmemRef<Type> operator[](uint16_t index) const {
        return ProgmemRef<Type>(*(ptr + index));
    }
    
    constexpr const Type* get_ptr() const {
      return ptr;
    }

    const uint16_t size;

private:
    const Type* const ptr;

};

template<typename Type>
class ProgmemPtr {
public:
    using type = Type;

    constexpr ProgmemPtr(): ptr(nullptr) {}
    constexpr ProgmemPtr(const Type* const ref): ptr(ref) {}
    constexpr ProgmemPtr(const ProgmemPtr& ref): ptr(ref.ptr) {}

    operator bool() const noexcept {
        return ptr;
    }
    void operator=(const ProgmemPtr& ref) noexcept {
        ptr = ref.ptr;
    }
    bool operator==(const Type* ref) const noexcept {
        return ref == ptr;
    }
    bool operator!=(const Type* ref) const noexcept {
        return ref != ptr;
    }
    bool operator==(ProgmemPtr ref) const noexcept {
        return ref.ptr == ptr;
    }
    bool operator!=(ProgmemPtr ref) const noexcept {
        return ref.ptr != ptr;
    }
    const ProgmemRef<Type> operator*() const noexcept {
        return (*this)[0];
    }

    constexpr ProgmemRef<Type> operator[](uint16_t index) const {
        return ProgmemRef<Type>(*(ptr + index));
    }
    
    constexpr const Type* get_ptr() const {
      return ptr;
    }
    
private:
    const Type* ptr;

};



template<typename _Ts, typename _Tf>
constexpr const ProgmemRef<const _Tf> access(const ProgmemPtr<const _Ts> ptr, _Tf const _Ts::* fptr) {
    return ProgmemRef<const _Tf>(ptr.get_ptr()->*fptr);
}

template<typename _Ts, typename _Tf>
constexpr const ProgmemPtr<const _Tf> access(const ProgmemPtr<const _Ts> ptr, const _Tf* const _Ts::* fptr) {
    return ProgmemPtr<const _Tf>(ProgmemRef<const _Tf*>(ptr.get_ptr()->*fptr).read());
}

template<typename _Ts, typename _Tf>
constexpr const ProgmemRef<const _Tf> access(const ProgmemRef<const _Ts> ptr, _Tf const _Ts::* fptr) {
    return ProgmemRef<const _Tf>(ptr.get_ptr()->*fptr);
}

template<typename _Ts, typename _Tf>
constexpr const ProgmemPtr<const _Tf> access(const ProgmemRef<const _Ts> ptr, const _Tf* const _Ts::* fptr) {
    return ProgmemPtr<const _Tf>(ProgmemRef<const _Tf*>(ptr.get_ptr()->*fptr).read());
}



