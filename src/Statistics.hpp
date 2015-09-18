#ifndef CLANGDUMPNAMES_STATISTICS_HPP
#define CLANGDUMPNAMES_STATISTICS_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>

#define BOOST_NO_RTTI
#define BOOST_NO_TYPEID
#define BOOST_NO_EXCEPTIONS
#define BOOST_EXCEPTION_DISABLE

#include <boost/property_tree/ptree_fwd.hpp>
#undef BOOST_PROPERTY_TREE_THROW
#define BOOST_PROPERTY_TREE_THROW(e)
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class Statistics {
    using typeType = std::string;
    using nameType = std::string;
    using countType = std::size_t;
    std::map<typeType, std::map<nameType, countType>> storage;

public:
    void addVariableOccurence(const clang::VarDecl* declaration) {
        nameType name_ = declaration->getNameAsString();

        clang::QualType qualifiedType = declaration->getType();
        typeType type_ = qualifiedType.getAsString();

        ++storage[type_][name_];
    }

    static Statistics createFromDump(const std::string& filename) {
        return Statistics{};
    }

    void dumpToFile(const std::string& filename) const {
        boost::property_tree::ptree ptree;

        const static std::string keyName = "Occurences";

        for (const auto& type : storage) {
            const auto& typeName = type.first;
            const auto& occurences = type.second;

            boost::property_tree::ptree typeTree;

            for (const auto& occurence : occurences) {
                const auto& variableName = occurence.first;
                const auto& nameCount = occurence.second;

                typeTree.put(variableName, nameCount);
            }

            ptree.put_child(typeName, typeTree);
        }

        std::ofstream outputFile("varnamedump.json");

        boost::property_tree::write_json(outputFile, ptree);
    }
};

#endif  // CLANGDUMPNAMES_STATISTICS_HPP
