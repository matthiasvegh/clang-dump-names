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

std::string getCurrentWorkingDir() {
  char path[255];
  return ::getcwd(path, 255) ? std::string(path) : "";
}

class VariableNameVisitor
    : public clang::RecursiveASTVisitor<VariableNameVisitor> {
  public:
  explicit VariableNameVisitor(clang::ASTContext* context,
                               Statistics& statistics,
                               const clang::SourceManager& sourceManager,
                               std::string currentWorkingDir)
      : context(context),
        statistics(statistics),
        sourceManager(sourceManager),
        currentWorkingDir(std::move(currentWorkingDir)) {}

  bool VisitDecl(clang::Decl* declaration) const {
    if (const clang::VarDecl* variableDeclaration =
            clang::dyn_cast<clang::VarDecl>(declaration)) {
      const auto& sourceRange = variableDeclaration->getSourceRange();
      const auto& sourceBegining = sourceRange.getBegin();
      const auto& fileName = sourceBegining.printToString(sourceManager);
      if(fileName[0] != '/') {
        statistics.addVariableOccurence(variableDeclaration);
      }
    }
    return true;
  }

  private:
  clang::ASTContext* context;
  Statistics& statistics;
  const clang::SourceManager& sourceManager;
  std::string currentWorkingDir;
};

class VariableNameDumperConsumer : public clang::ASTConsumer {
  clang::CompilerInstance& instance;

  public:
  VariableNameDumperConsumer(clang::CompilerInstance& instance,
                             Statistics statistics,
                             const clang::SourceManager& sourceManager)
      : instance(instance),
        statistics(std::move(statistics)),
        sourceManager(sourceManager),
        currentWorkingDir(getCurrentWorkingDir()),
        visitor(&instance.getASTContext(), this->statistics,
                this->sourceManager, this->currentWorkingDir) {}

  void HandleTranslationUnit(clang::ASTContext& context) override {
    visitor.TraverseDecl(context.getTranslationUnitDecl());
    statistics.dumpToFile("varnamedump.json");
  }

  private:
  Statistics statistics;
  const clang::SourceManager& sourceManager;
  std::string currentWorkingDir;
  VariableNameVisitor visitor;
};

class VariableNameDumperAction : public clang::PluginASTAction {
  public:
  clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance& ci,
                                        llvm::StringRef) override {
    const auto& sourceManager = ci.getSourceManager();
    auto statistics = Statistics::createFromDump("varnamedump.json");
    return new VariableNameDumperConsumer(ci, std::move(statistics),
                                          sourceManager);
  }

  bool ParseArgs(const clang::CompilerInstance& /*ci*/,
                 const std::vector<std::string>& args) override {
    for (const auto& arg : args) {
      std::cerr << arg << std::endl;
    }
    return true;
  }
};

}  // unnamed namespace

static clang::FrontendPluginRegistry::Add<VariableNameDumperAction> X(
    "dump-names", "Dump variable names");
