#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/STLExtras.h"

#define BOOST_NO_RTTI
#define BOOST_NO_TYPEID

#include "Statistics.hpp"

namespace {

std::string getTypeName(const clang::QualType& type) {
  return type.getAsString();
}

class VariableNameVisitor
    : public clang::RecursiveASTVisitor<VariableNameVisitor> {
  public:
  explicit VariableNameVisitor(clang::ASTContext* context,
                               Statistics& statistics)
      : context(context), statistics(statistics) {}

  bool VisitDecl(clang::Decl* declaration) const {
    if (const clang::VarDecl* variableDeclaration =
            clang::dyn_cast<clang::VarDecl>(declaration)) {
      statistics.addVariableOccurence(variableDeclaration);
    }
    return true;
  }

  private:
  clang::ASTContext* context;
  Statistics& statistics;
};

class VariableNameDumperConsumer : public clang::ASTConsumer {
  clang::CompilerInstance& instance;

  public:
  VariableNameDumperConsumer(clang::CompilerInstance& instance,
                             Statistics& statistics)
      : instance(instance),
        visitor(&instance.getASTContext(), statistics),
        statistics(statistics) {}

  void HandleTranslationUnit(clang::ASTContext& context) override {
    visitor.TraverseDecl(context.getTranslationUnitDecl());
    statistics.dumpToFile("varnamedump.json");
  }

  private:
  VariableNameVisitor visitor;
  Statistics& statistics;
};

static Statistics _global_statistics =
    Statistics::createFromDump("varnamedump.json");


class VariableNameDumperAction : public clang::PluginASTAction {
  public:
  clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance& ci,
                                        llvm::StringRef) override {
    return new VariableNameDumperConsumer(ci, _global_statistics);
  }

  bool ParseArgs(const clang::CompilerInstance& /*ci*/,
                 const std::vector<std::string>& /*args*/) override {
    return true;
  }
};

}  // unnamed namespace

static clang::FrontendPluginRegistry::Add<VariableNameDumperAction> X(
    "dump-names", "Dump variable names");
