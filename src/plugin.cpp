#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/STLExtras.h"

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
                             Statistics statistics)
      : instance(instance),
        statistics(std::move(statistics)),
        visitor(&instance.getASTContext(), this->statistics) {}

  void HandleTranslationUnit(clang::ASTContext& context) override {
    visitor.TraverseDecl(context.getTranslationUnitDecl());
    statistics.dumpToFile("varnamedump.json");
  }

  private:
  Statistics statistics;
  VariableNameVisitor visitor;
};

class VariableNameDumperAction : public clang::PluginASTAction {
  public:
  clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance& ci,
                                        llvm::StringRef) override {
    auto statistics = Statistics::createFromDump("varnamedump.json");
    return new VariableNameDumperConsumer(ci, std::move(statistics));
  }

  bool ParseArgs(const clang::CompilerInstance& /*ci*/,
                 const std::vector<std::string>& /*args*/) override {
    return true;
  }
};

}  // unnamed namespace

static clang::FrontendPluginRegistry::Add<VariableNameDumperAction> X(
    "dump-names", "Dump variable names");
