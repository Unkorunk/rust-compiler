#pragma once

#include <map>

#include "Symbol.hpp"
#include "WasmTypes.hpp"

class TypesHelper {
public:
    static const semantic::DefaultType &ConvertToDefaultType(const std::string &type) {
        return kDefaultTypes.at(type);
    }

    static std::string ConvertToString(TokenValue::Type type) {
        return kRawTypeToStr.at(type);
    }

    static const semantic::DefaultType &ConvertToDefaultType(TokenValue::Type type) {
        return ConvertToDefaultType(ConvertToString(type));
    }

    static const TokenValue::Type ConvertToRawType(const std::string &type) {
        return kStrToRawType.at(type);
    }

    static std::vector<wasm::ValueType> ToWasmTypes(const std::vector<TokenValue::Type> &types) {
        std::vector<wasm::ValueType> result;
        result.reserve(types.size());
        for (TokenValue::Type type : types) {
            result.push_back(TypesHelper::ConvertToWasm(type));
        }
        return result;
    }

    static wasm::ValueType ConvertToWasm(TokenValue::Type type) {
        switch (type) {
        case TokenValue::Type::kI32:
            return wasm::ValueType::i32;
        case TokenValue::Type::kI64:
            return wasm::ValueType::i64;
        case TokenValue::Type::kF32:
            return wasm::ValueType::f32;
        case TokenValue::Type::kF64:
            return wasm::ValueType::f64;
        default:
            throw std::exception();  // todo
        }
    }

    const static std::map<std::string, semantic::DefaultType> kDefaultTypes;
    const static std::map<TokenValue::Type, std::string> kRawTypeToStr;
    const static std::map<std::string, TokenValue::Type> kStrToRawType;
};

const std::map<std::string, semantic::DefaultType> TypesHelper::kDefaultTypes{
    {"bool", semantic::DefaultType(TokenValue::Type::kBool)}, {"char", semantic::DefaultType(TokenValue::Type::kChar)}, {"u8", semantic::DefaultType(TokenValue::Type::kU8)},
    {"u16", semantic::DefaultType(TokenValue::Type::kU16)},   {"u32", semantic::DefaultType(TokenValue::Type::kU32)},   {"u64", semantic::DefaultType(TokenValue::Type::kU64)},
    {"i8", semantic::DefaultType(TokenValue::Type::kI8)},     {"i16", semantic::DefaultType(TokenValue::Type::kI16)},   {"i32", semantic::DefaultType(TokenValue::Type::kI32)},
    {"i64", semantic::DefaultType(TokenValue::Type::kI64)},   {"f32", semantic::DefaultType(TokenValue::Type::kF32)},   {"f64", semantic::DefaultType(TokenValue::Type::kF64)},
    {"str", semantic::DefaultType(TokenValue::Type::kText)},  {"usize", semantic::DefaultType(TokenValue::Type::kU64)}, {"isize", semantic::DefaultType(TokenValue::Type::kI64)},
    {"void", semantic::DefaultType(TokenValue::Type::kVoid)}};

const std::map<TokenValue::Type, std::string> TypesHelper::kRawTypeToStr{
    {TokenValue::Type::kBool, "bool"}, {TokenValue::Type::kU16, "u16"}, {TokenValue::Type::kI8, "i8"},     {TokenValue::Type::kI64, "i64"}, {TokenValue::Type::kChar, "char"},
    {TokenValue::Type::kU32, "u32"},   {TokenValue::Type::kI16, "i16"}, {TokenValue::Type::kF32, "f32"},   {TokenValue::Type::kU8, "u8"},   {TokenValue::Type::kU64, "u64"},
    {TokenValue::Type::kI32, "i32"},   {TokenValue::Type::kF64, "f64"}, {TokenValue::Type::kVoid, "void"}, {TokenValue::Type::kText, "str"}};

const std::map<std::string, TokenValue::Type> TypesHelper::kStrToRawType{
    {"bool", TokenValue::Type::kBool}, {"u16", TokenValue::Type::kU16}, {"i8", TokenValue::Type::kI8},     {"i64", TokenValue::Type::kI64}, {"char", TokenValue::Type::kChar},
    {"u32", TokenValue::Type::kU32},   {"i16", TokenValue::Type::kI16}, {"f32", TokenValue::Type::kF32},   {"u8", TokenValue::Type::kU8},   {"u64", TokenValue::Type::kU64},
    {"i32", TokenValue::Type::kI32},   {"f64", TokenValue::Type::kF64}, {"void", TokenValue::Type::kVoid}, {"str", TokenValue::Type::kText}};