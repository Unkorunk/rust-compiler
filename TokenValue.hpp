#pragma once

#include <cstdint>

class TokenValue {
public:
    enum class Type {
        kBool, kChar, kU8, kU16, kU32, kU64, kU128, kUSize,
        kI8, kI16, kI32, kI64, kI128, kISize,
        kF32, kF64, kText, kEmpty
    };

    TokenValue() : type_(Type::kEmpty) {}

    TokenValue(bool val) : _bool(val), type_(Type::kBool) {}

    TokenValue(char val) : _char(val), type_(Type::kChar) {}

    TokenValue(uint8_t val) : _u8(val), type_(Type::kU8) {}
    TokenValue(uint16_t val) : _u16(val), type_(Type::kU16) {}
    TokenValue(uint32_t val) : _u32(val), type_(Type::kU32) {}
    TokenValue(uint64_t val) : _u64(val), type_(Type::kU64) {}
    // TODO u128 - not implemented

    TokenValue(int8_t val) : _i8(val), type_(Type::kI8) {}
    TokenValue(int16_t val) : _i16(val), type_(Type::kI16) {}
    TokenValue(int32_t val) : _i32(val), type_(Type::kI32) {}
    TokenValue(int64_t val) : _i64(val), type_(Type::kI64) {}
    // TODO i128 - not implemented

    TokenValue(float val) : _f32(val), type_(Type::kF32) {}
    TokenValue(double val) : _f64(val), type_(Type::kF64) {}

    TokenValue(std::string val) : _text(val), type_(Type::kText) {}

    operator bool() const {
        if (type_ != Type::kBool) throw std::exception();
        return _bool;
    }

    operator char() const {
        if (type_ != Type::kChar) throw std::exception();
        return _char;
    }

    operator uint8_t() const {
        if (type_ != Type::kU8) throw std::exception();
        return _u8;
    }
    operator uint16_t() const {
        if (type_ != Type::kU16) throw std::exception();
        return _u16;
    }
    operator uint32_t() const {
        if (type_ != Type::kU32) throw std::exception();
        return _u32;
    }
    operator uint64_t() const {
        if (type_ != Type::kU64) throw std::exception();
        return _u64;
    }
    // TODO u128 - not implemented
    operator int8_t() const {
        if (type_ != Type::kI8) throw std::exception();
        return _i8;
    }
    operator int16_t() const {
        if (type_ != Type::kI16) throw std::exception();
        return _i16;
    }
    operator int32_t() const {
        if (type_ != Type::kI32) throw std::exception();
        return _i32;
    }
    operator int64_t() const {
        if (type_ != Type::kI64) throw std::exception();
        return _i64;
    }
    // TODO i128 - not implemented
    operator float() const {
        if (type_ != Type::kF32) throw std::exception();
        return _f32;
    }
    operator double() const {
        if (type_ != Type::kF64) throw std::exception();
        return _f64;
    }

    operator std::string() const {
        if (type_ != Type::kText) throw std::exception();
        return _text;
    }

    std::string ToString() const {
        std::ostringstream oss;

        oss << TypeToString(type_);
        if (type_ != Type::kEmpty) {
            switch (type_)
            {
            case Type::kBool:
                oss << ' ' << _bool;
                break;
            case Type::kChar:
                oss << ' ' << _char;
                break;
            case Type::kU8:
                oss << ' ' << static_cast<uint16_t>(_u8);
                break;
            case Type::kU16:
                oss << ' ' << _u16;
                break;
            case Type::kU32:
                oss << ' ' << _u32;
                break;
            case Type::kU64:
                oss << ' ' << _u64;
                break;
            case Type::kU128:
                throw std::exception(); // TODO not implemented
            case Type::kI8:
                oss << ' ' << static_cast<int16_t>(_i8);
                break;
            case Type::kI16:
                oss << ' ' << _i16;
                break;
            case Type::kI32:
                oss << ' ' << _i32;
                break;
            case Type::kI64:
                oss << ' ' << _i64;
                break;
            case Type::kI128:
                throw std::exception(); // TODO not implemented
            case Type::kF32:
                oss << ' ' << _f32;
                break;
            case Type::kF64:
                oss << ' ' << _f64;
                break;
            case Type::kText:
                oss << ' ' << _text;
                break;
            default:
                throw std::exception();
            }
        }

        return oss.str();
    }

    static std::string TypeToString(Type type) {
        switch (type)
        {
        case Type::kBool:
            return "bool";
        case Type::kChar:
            return "char";
        case Type::kU8:
            return "u8";
        case Type::kU16:
            return "u16";
        case Type::kU32:
            return "u32";
        case Type::kU64:
            return "u64";
        case Type::kU128:
            return "u128";
        case Type::kI8:
            return "i8";
        case Type::kI16:
            return "i16";
        case Type::kI32:
            return "i32";
        case Type::kI64:
            return "i64";
        case Type::kI128:
            return "i128";
        case Type::kF32:
            return "f32";
        case Type::kF64:
            return "f64";
        case Type::kText:
            return "text";
        case Type::kEmpty:
            return "empty";
        default:
            throw std::exception();
        }
    }
private:
    Type type_;

    union {
        bool _bool;

        char _char;

        uint8_t _u8;
        uint16_t _u16;
        uint32_t _u32;
        uint64_t _u64;

        int8_t _i8;
        int16_t _i16;
        int32_t _i32;
        int64_t _i64;

        float _f32;
        double _f64;
    };

    std::string _text;
};
