#include <filesystem>
#include <fstream>
#include <iostream>

#include "SemanticAnalyzer.hpp"
#include "SpecificSyntaxTreeVisitor.hpp"
#include "SyntaxParser.hpp"
#include "Tokenizer.hpp"
#include "WasmGenerator.hpp"

class MyVisitor : public SpecificSyntaxTreeVisitor {
protected:
    int indent = 0;
    void PrintIndent() const {
        for (int i = 0; i < indent; i++) {
            std::cout << ' ';
        }
    }

    void PostVisit(const IdentifierNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << node->GetToken()->GetTokenValue().ToString() << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const LiteralNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << node->GetToken()->GetTokenValue().ToString() << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ParamFunctionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ParamFunctionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ParamStructNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ParamStructNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const LetNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "LetNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const FunctionNode *node) override {
        indent += 2;

        PrintIndent();
        if (node->IsConst()) {
            std::cout << "const ";
        }
        std::cout << "FunctionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const StructNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "StructNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ConstantItemNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ConstantItemNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ParenthesizedTypeNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ParenthesizedTypeNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const TupleTypeNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleTypeNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ReferenceTypeNode *node) override {
        indent += 2;

        PrintIndent();
        if (node->IsMut()) {
            std::cout << "mut ";
        }
        std::cout << "ReferenceTypeNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ArrayTypeNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ArrayTypeNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IdentifierTypeNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

    void PostVisit(const TupleIndexFieldNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleIndexFieldNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IdentifierFieldNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IdentifierFieldNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const RefMutIdentifierFieldNode *node) override {
        indent += 2;

        PrintIndent();
        if (node->IsRef()) {
            std::cout << "ref ";
        }
        if (node->IsMut()) {
            std::cout << "mut ";
        }
        std::cout << "RefMutIdentifierFieldNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const LiteralPatternNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

    void PostVisit(const IdentifierPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IdentifierPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const WildcardPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "WildcardPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const RestPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "RestPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ReferencePatternNode *node) override {
        indent += 2;

        PrintIndent();
        if (node->IsMut()) {
            std::cout << "mut ";
        }
        if (node->IsSingleRef()) {
            std::cout << "& ";
        } else {
            std::cout << "&& ";
        }
        std::cout << "ReferencePatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const StructPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "StructPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const TupleStructPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleStructPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const TuplePatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TuplePatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const GroupedPatternNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "GroupedPatternNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IdentifierExpressionNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

    void PostVisit(const LiteralExpressionNode *node) override {
        SpecificSyntaxTreeVisitor::PostVisit(node);
    }

    void PostVisit(const BinaryOperationNode *node) override {
        indent += 2;
        const auto token = node->GetToken();
        const auto tokenType = token->GetType();

        PrintIndent();
        std::cout << Token::TypeToString(tokenType) << " BinaryOperationNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const PrefixUnaryOperationNode *node) override {
        indent += 2;

        const auto token = node->GetToken();
        const auto tokenType = token->GetType();

        PrintIndent();
        if (node->IsException()) {
            switch (node->GetException()) {
            case PrefixUnaryOperationNode::Exception::kAndMut:
                std::cout << "&mut";
                break;
            default:
                throw std::exception();
            }
        } else {
            std::cout << Token::TypeToString(tokenType);
        }
        std::cout << " PrefixUnaryOperationNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const InfiniteLoopNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "InfiniteLoopNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const PredicateLoopNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "PredicateLoopNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IteratorLoopNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IteratorLoopNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IfNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IfNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const BlockNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "BlockNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const BreakNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "BreakNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ContinueNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ContinueNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ReturnNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ReturnNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const CallOrInitTupleNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "CallOrInitTupleNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IndexNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IndexNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const MemberAccessNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "MemberAccessNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }
    void PostVisit(const ArrayExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        if (node->IsSemiMode()) {
            std::cout << "; ";
        }
        std::cout << "ArrayExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const InitStructExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "InitStructExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const ShorthandFieldInitStructExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "ShorthandFieldInitStructExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const TupleIndexFieldInitStructExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleIndexFieldInitStructExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const IdentifierFieldInitStructExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "IdentifierFieldInitStructExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const TupleExpressionNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "TupleExpressionNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const SyntaxTree *node) override {
        indent += 2;

        PrintIndent();
        std::cout << "SyntaxTree" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }

    void PostVisit(const AssignmentNode *node) override {
        indent += 2;

        PrintIndent();
        std::cout << Token::TypeToString(node->GetOperation().GetType()) << " AssignmentNode" << std::endl;

        SpecificSyntaxTreeVisitor::PostVisit(node);

        indent -= 2;
    }
};

int main(int argc, char **argv) {
    bool print_tokenizer = false;
    bool print_syntax = false;
    bool print_semantic = false;

    std::string filename;
    bool filename_found = false;

    for (int k = 1; k < argc; k++) {
        std::string arg(argv[k]);

        if (arg == "-t") {
            print_tokenizer = true;
        } else if (arg == "-s") {
            print_syntax = true;
        } else if (arg == "-m") {
            print_semantic = true;
        } else if (filename_found) {
            std::cerr << "invalid arguments" << std::endl;
            return 0;
        } else {
            filename = arg;
            filename_found = true;
        }
    }

    if (!filename_found) {
        std::cerr << "no input file" << std::endl;
        return 0;
    }

    std::filesystem::path p1(filename);
    std::filesystem::path json_filename = p1.stem();
    json_filename.replace_extension("json");
    p1.replace_filename(json_filename);

    ImportExportTable import_export_table(p1.string());

    std::ifstream ifs(filename);

    Tokenizer tokenizer(&ifs, Tokenizer::TargetType::kX64);
    if (print_tokenizer) {
        while (tokenizer.HasNext()) {
            Token token = tokenizer.Next();
            if (token.GetType() == Token::Type::kEndOfFile) {
                break;
            }

            std::cout << token.ToString(&ifs) << std::endl;
        }

        ifs.close();
        return 0;
    }

    SyntaxParser parser(&tokenizer);
    std::unique_ptr<SyntaxTree> syntax_tree = parser.ParseItems();

    if (print_syntax) {
        MyVisitor visitor;
        visitor.Visit(syntax_tree.get());
    }

    semantic::SemanticAnalyzer analyzer;
    analyzer.Analyze(syntax_tree.get(), &import_export_table);

    if (print_semantic) {
        syntax_tree->symbol_table->Print();
    }

    WasmGenerator generator;
    generator.Generate(syntax_tree.get(), &import_export_table);
    ByteArray::FileStream fs("index.wasm", std::ios::out | std::ios::binary);
    fs << generator.GetResult();

    std::ofstream index_ofs("index.html");
    index_ofs << R"(<!doctype html>
<html>
  <body>
    <script>
      var importObject = {
)";

    std::unordered_map<std::string, std::vector<const ImportExportTable::Import *>> grouped;

    for (const auto &it : import_export_table.imports) {
        grouped[it.module].push_back(&it);
    }

    for (const auto&[group, imports] : grouped) {
        index_ofs << "          " << group << ": {" << std::endl;

        for (const auto &it : imports) {
            index_ofs << "              " << it->field << ": (";
            auto params = it->type.GetWasmParams();
            for (size_t i = 0; i < params.size(); i++) {
                index_ofs << "arg" << i;
                if (i != params.size() - 1) {
                    index_ofs << ", ";
                }
            }
            index_ofs << ") => { /* todo */ }," << std::endl;
        }

        index_ofs << "          }," << std::endl;
    }

    index_ofs << R"(      };

      WebAssembly.instantiateStreaming(fetch('index.wasm'), importObject).then(obj => {
        // todo
      });
    </script>
  </body>
</html>)";

    index_ofs.close();

    ifs.close();
    return 0;
}
