#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/STLExtras.h"

namespace {

class VariableNameVisitor
    : public clang::RecursiveASTVisitor<VariableNameVisitor> {
  public:
  explicit VariableNameVisitor(clang::ASTContext* context) : context(context) {}

  bool VisitDecl(clang::Decl* declaration) const {
    if (const clang::VarDecl* variableDeclaration =
            clang::dyn_cast<clang::VarDecl>(declaration)) {
      llvm::errs() << variableDeclaration->getNameAsString() << "\n";
      auto type = variableDeclaration->getType();
      type->dump();
    }
    return true;
  }

  private:
  clang::ASTContext* context;
};

class VariableNameDumperConsumer : public clang::ASTConsumer {
  clang::CompilerInstance& instance;

  public:
  VariableNameDumperConsumer(clang::CompilerInstance& instance)
      : instance(instance), visitor(&instance.getASTContext()) {
    llvm::errs() << "Consumer online\n";
  }

  void HandleTranslationUnit(clang::ASTContext& context) override {
    llvm::errs() << "Translation unit over\n";
    visitor.TraverseDecl(context.getTranslationUnitDecl());
  }

  private:
  VariableNameVisitor visitor;
};

class VariableNameDumperAction : public clang::PluginASTAction {
  public:
  clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance& ci,
                                        llvm::StringRef) override {
    return new VariableNameDumperConsumer(ci);
  }

  bool ParseArgs(const clang::CompilerInstance& /*ci*/,
                 const std::vector<std::string>& /*args*/) override {
    llvm::errs() << "Parsed args\n";
    return true;
  }
};

}  // unnamed namespace

static clang::FrontendPluginRegistry::Add<VariableNameDumperAction> X(
    "dump-names", "Dump variable names");
