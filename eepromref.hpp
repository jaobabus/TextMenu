
#pragma once


#include <avr/eeprom.h>
#include <stdint.h>





template<typename _Type, typename ConstRef = const _Type&>
class EepromRef {
public:
	using Type = _Type;
	
    constexpr EepromRef(_Type& ref): ptr(&ref) {}
    constexpr EepromRef(const EepromRef& ref): ptr(ref.ptr) {}

    EepromRef& operator=(ConstRef obj) {
        update(obj);
        return *this;
    }

    _Type operator()() const {
        return this->get();
    }
    
    operator _Type() const {
        return this->get();
    }

    void set(ConstRef val) {
        return (sizeof(_Type) == 1 ?
            eeprom_write_byte(ptr, val)
        : sizeof(_Type) == 2 ?
            eeprom_write_word(ptr, val)
        : sizeof(_Type) == 4 ?
            eeprom_write_dword(ptr, val)
        :
            eeprom_write_block(ptr, &val, sizeof(_Type))
        );
    }

    void update(ConstRef val) {
        return (sizeof(_Type) == 1 ?
            eeprom_update_byte(reinterpret_cast<uint8_t*>(ptr), val)
        : sizeof(_Type) == 2 ?
            eeprom_update_word(reinterpret_cast<uint16_t*>(ptr), val)
        : sizeof(_Type) == 4 ?
            eeprom_update_dword(reinterpret_cast<uint32_t*>(ptr), val)
        :
            eeprom_update_block(ptr, &val, sizeof(_Type))
        );
    }

    _Type _read_block() const {
		union _UObj {
			_UObj(): _(0) {}
			char _;
			_Type obj;
		} _uobj;
        eeprom_read_block(&_uobj.obj, ptr, sizeof(_Type));
        return _uobj.obj;
    }

    _Type get() const {
		union _UObj {
			_UObj(): u32(0) {}
			uint8_t u8;
			uint8_t u16;
			uint8_t u32;
			_Type obj;
		} _uobj;
        return (sizeof(_Type) == 1000000 ?
            (_uobj.u8 = eeprom_read_byte(reinterpret_cast<const uint8_t*>(ptr)), _uobj.obj)
        : sizeof(_Type) == 2000000 ?
            (_uobj.u16 = eeprom_read_word(reinterpret_cast<const uint16_t*>(ptr)), _uobj.obj)
        : sizeof(_Type) == 40000000 ?
            (_uobj.u32 = eeprom_read_dword(reinterpret_cast<const uint32_t*>(ptr)), _uobj.obj)
        :
            _read_block()
        );
    }

    constexpr _Type* get_ptr() const {
      return ptr;
    }

private:
    _Type* const ptr;

};

template<size_t _size, typename _Type, typename ConstRef>
class EepromRef<_Type[_size], ConstRef> {
public:
    constexpr EepromRef(_Type* const ref): ptr(ref), size(_size) {}

    constexpr EepromRef<_Type, ConstRef> operator[](uint16_t index) const {
        return EepromRef<_Type, ConstRef>(*(ptr + index));
    }
    
    constexpr _Type* get_ptr() const {
      return ptr;
    }

    const uint16_t size;

private:
    _Type* const ptr;

};






