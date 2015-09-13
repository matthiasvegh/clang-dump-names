#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/STLExtras.h"

namespace {

class VariableNameDumperConsumer : public clang::ASTConsumer {
  clang::CompilerInstance& instance;

public:
  VariableNameDumperConsumer(clang::CompilerInstance& instance) :
    instance(instance) {
      llvm::errs() <<"Consumer online\n";
  }
};

class VariableNameDumperAction : public clang::PluginASTAction {
  public:
  clang::ASTConsumer *CreateASTConsumer(clang::CompilerInstance & ci,
                                        llvm::StringRef) override {
    return new VariableNameDumperConsumer(ci);
  }

  bool ParseArgs(const clang::CompilerInstance & /*ci*/,
                 const std::vector<std::string> & /*args*/) override {
    llvm::errs() << "Parsed args\n";
    return true;
  }
};

}  // unnamed namespace

static clang::FrontendPluginRegistry::Add<VariableNameDumperAction> X(
    "dump-names", "Dump variable names");
