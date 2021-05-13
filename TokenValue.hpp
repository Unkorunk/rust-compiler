#pragma once

#include <cstdint>
#include <vector>

class TokenValue {
public:
    enum class Type
    {
        kBool,
        kChar,
        kU8,
        kU16,
        kU32,
        kU64,
        kI8,
        kI16,
        kI32,
        kI64,
        kF32,
        kF64,
        kText,
        kByteString,
        kEmpty
    };

    TokenValue() : type_(Type::kEmpty) {}

    TokenValue(bool val) : bool_(val), type_(Type::kBool) {}

    TokenValue(char val) : char_(val), type_(Type::kChar) {}

    TokenValue(uint8_t val) : u8_(val), type_(Type::kU8) {}
    TokenValue(uint16_t val) : u16_(val), type_(Type::kU16) {}
    TokenValue(uint32_t val) : u32_(val), type_(Type::kU32) {}
    TokenValue(uint64_t val) : u64_(val), type_(Type::kU64) {}

    TokenValue(int8_t val) : i8_(val), type_(Type::kI8) {}
    TokenValue(int16_t val) : i16_(val), type_(Type::kI16) {}
    TokenValue(int32_t val) : i32_(val), type_(Type::kI32) {}
    TokenValue(int64_t val) : i64_(val), type_(Type::kI64) {}

    TokenValue(float val) : f32_(val), type_(Type::kF32) {}
    TokenValue(double val) : f64_(val), type_(Type::kF64) {}

    TokenValue(std::string val) : text_(val), type_(Type::kText) {}

    TokenValue(const std::vector<uint8_t> &val) : byte_string_(val), type_(Type::kByteString) {}

    operator bool() const {
        if (type_ != Type::kBool)
            throw std::exception();
        return bool_;
    }

    operator char() const {
        if (type_ != Type::kChar)
            throw std::exception();
        return char_;
    }

    operator uint8_t() const {
        if (type_ != Type::kU8)
            throw std::exception();
        return u8_;
    }

    operator uint16_t() const {
        if (type_ != Type::kU16)
            throw std::exception();
        return u16_;
    }

    operator uint32_t() const {
        if (type_ != Type::kU32)
            throw std::exception();
        return u32_;
    }

    operator uint64_t() const {
        if (type_ != Type::kU64)
            throw std::exception();
        return u64_;
    }

    operator int8_t() const {
        if (type_ != Type::kI8)
            throw std::exception();
        return i8_;
    }

    operator int16_t() const {
        if (type_ != Type::kI16)
            throw std::exception();
        return i16_;
    }

    operator int32_t() const {
        if (type_ != Type::kI32)
            throw std::exception();
        return i32_;
    }

    operator int64_t() const {
        if (type_ != Type::kI64)
            throw std::exception();
        return i64_;
    }

    operator float() const {
        if (type_ != Type::kF32)
            throw std::exception();
        return f32_;
    }

    operator double() const {
        if (type_ != Type::kF64)
            throw std::exception();
        return f64_;
    }

    operator std::string() const {
        if (type_ != Type::kText)
            throw std::exception();
        return text_;
    }

    operator std::vector<uint8_t>() const {
        if (type_ != Type::kByteString)
            throw std::exception();
        return byte_string_;
    }

    std::string ToString() const {
        std::ostringstream oss;

        oss << TypeToString(type_);
        if (type_ != Type::kEmpty) {
            oss << ' ' << ValueToString();
        }

        return oss.str();
    }

    std::string ValueToString() const {
        std::ostringstream oss;
        if (type_ != Type::kEmpty) {
            switch (type_) {
            case Type::kBool:
                oss << bool_;
                break;
            case Type::kChar:
                oss << char_;
                break;
            case Type::kU8:
                oss << static_cast<uint16_t>(u8_);
                break;
            case Type::kU16:
                oss << u16_;
                break;
            case Type::kU32:
                oss << u32_;
                break;
            case Type::kU64:
                oss << u64_;
                break;
            case Type::kI8:
                oss << static_cast<int16_t>(i8_);
                break;
            case Type::kI16:
                oss << i16_;
                break;
            case Type::kI32:
                oss << i32_;
                break;
            case Type::kI64:
                oss << i64_;
                break;
            case Type::kF32:
                oss << f32_;
                break;
            case Type::kF64:
                oss << f64_;
                break;
            case Type::kText:
                oss << text_;
                break;
            case Type::kByteString:
                for (size_t i = 0; i < byte_string_.size(); i++) {
                    oss << static_cast<uint16_t>(byte_string_[i]);
                    if (i + 1 != byte_string_.size()) {
                        oss << ' ';
                    }
                }
                break;
            default:
                throw std::exception();
            }
        }
        return oss.str();
    }

    static std::string TypeToString(Type type) {
        switch (type) {
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
        case Type::kI8:
            return "i8";
        case Type::kI16:
            return "i16";
        case Type::kI32:
            return "i32";
        case Type::kI64:
            return "i64";
        case Type::kF32:
            return "f32";
        case Type::kF64:
            return "f64";
        case Type::kText:
            return "text";
        case Type::kByteString:
            return "byte string";
        case Type::kEmpty:
            return "empty";
        default:
            throw std::exception();
        }
    }

private:
    Type type_;

    union {
        bool bool_;

        char char_;

        uint8_t u8_;
        uint16_t u16_;
        uint32_t u32_;
        uint64_t u64_;

        int8_t i8_;
        int16_t i16_;
        int32_t i32_;
        int64_t i64_;

        float f32_;
        double f64_;
    };

    std::string text_;
    std::vector<uint8_t> byte_string_;
};
