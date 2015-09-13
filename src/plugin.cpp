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
    llvm::errs() << "Visiting cxx declaration\n";
    if (const clang::NamedDecl* namedDeclaration =
            clang::dyn_cast<clang::NamedDecl>(declaration)) {
      llvm::errs() << namedDeclaration->getNameAsString() << "\n";
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

  bool HandleTopLevelDecl(clang::DeclGroupRef DG) override {
    llvm::errs() << "Top level called\n";
    for (const clang::Decl* declaration : DG) {
      llvm::errs() << "Element of group\n";
      if (const clang::NamedDecl* nd =
              clang::dyn_cast<clang::NamedDecl>(declaration)) {
        llvm::errs() << nd->getNameAsString() << "\n";
      }
    }
    return true;
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
