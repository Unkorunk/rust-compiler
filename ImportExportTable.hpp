#pragma once

#include <nlohmann/json.hpp>

#include "TokenValue.hpp"
#include "TypesHelper.hpp"

class ImportExportTable final {
public:
    struct Type {
        std::vector<TokenValue::Type> params;
        std::vector<TokenValue::Type> ret;

        std::vector<wasm::ValueType> GetWasmParams() const {
            return TypesHelper::ToWasmTypes(params);
        }

        std::vector<wasm::ValueType> GetWasmReturn() const {
            return TypesHelper::ToWasmTypes(ret);
        }
    };

    struct Import {
        std::string module;
        std::string field;
        Type type;
        std::string associate;
    };

    struct Export {
        std::string field;
        Type type;
        std::string associate;
    };

    explicit ImportExportTable(const std::string &filename) {
        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
            throw std::exception();  // todo
        }

        nlohmann::json j;
        ifs >> j;

        auto imports_array = j["imports"];
        for (auto it = imports_array.begin(); it != imports_array.end(); ++it) {
            Import import;

            import.module = (*it)["module"].get<std::string>();
            import.field = (*it)["field"].get<std::string>();

            auto array_obj = (*it)["type"]["params"];

            for (auto it4 = array_obj.begin(); it4 != array_obj.end(); ++it4) {
                import.type.params.push_back(TypesHelper::ConvertToRawType(it4->get<std::string>()));
            }

            array_obj = (*it)["type"]["return"];
            for (auto it4 = array_obj.begin(); it4 != array_obj.end(); ++it4) {
                import.type.ret.push_back(TypesHelper::ConvertToRawType(it4->get<std::string>()));
            }

            import.associate = (*it)["associate"].get<std::string>();

            imports.push_back(import);
        }

        auto exports_array = j["exports"];
        for (auto it = exports_array.begin(); it != exports_array.end(); ++it) {
            Export export_;

            export_.field = (*it)["field"].get<std::string>();

            auto array_obj = (*it)["type"]["params"];

            for (auto it4 = array_obj.begin(); it4 != array_obj.end(); ++it4) {
                export_.type.params.push_back(TypesHelper::ConvertToRawType(it4->get<std::string>()));
            }

            array_obj = (*it)["type"]["return"];
            for (auto it4 = array_obj.begin(); it4 != array_obj.end(); ++it4) {
                export_.type.ret.push_back(TypesHelper::ConvertToRawType(it4->get<std::string>()));
            }

            export_.associate = (*it)["associate"].get<std::string>();

            exports.push_back(export_);
        }

        ifs.close();
    }

    std::vector<Import> imports;
    std::vector<Export> exports;
};