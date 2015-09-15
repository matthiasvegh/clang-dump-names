#ifndef CLANGDUMPNAMES_STATISTICS_HPP
#define CLANGDUMPNAMES_STATISTICS_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>

class Statistics {
    using typeType = std::string;
    using nameType = std::string;
    using countType = std::size_t;
    std::map<typeType, std::map<nameType, countType>> storage;

public:
    void addVariableOccurence(const clang::VarDecl* declaration) {
        nameType name_ = declaration->getNameAsString();

        clang::QualType  qualifiedType = declaration->getType();
        typeType type_ = qualifiedType.getAsString();

        ++storage[type_][name_];
    }

    static Statistics createFromDump(const std::string& filename) {
        return Statistics{};
    }

    void dumpToFile(const std::string& filename) const {
        std::ofstream file;
        file.open(filename);

        file << "{\n";
        bool firstType = true;
        for(const auto& type: storage) {
            if (!firstType) file << ",\n";
            firstType = false;
            file << "    " << '"' << type.first << "\": {";

            for(const auto& names: type.second) {
                file << '"' << names.first << '"' << ": " << names.second;
            }

            file << "}";
        }
        file << "\n}\n";
    }

};

#endif // CLANGDUMPNAMES_STATISTICS_HPP
