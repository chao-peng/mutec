#include <string>
#include <map>
#include <list>
#include <iostream>
#include <sstream>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/Core/Replacement.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Basic/LLVM.h"

#include "SourceCodeInvastigator.h"
#include "Constants.h"
#include "Utils.h"

using namespace clang;
using namespace clang::tooling;

std::string codeTemplate;
std::map<std::string, std::string> templateMap;

std::map<std::string, unsigned int> operatorType;
std::map<std::string, std::list<std::string>> mutantOperator;

int currentOperator;
int numOperators;
int currentSourceFileID;
int numSourceFile;
std::string currentFileName;
std::map<int, std::string> mutableOperatorTemplates;
std::map<std::string, std::list<std::string>> generatedMutantList;

std::string directory;
bool randomGenerate;
bool generateTemplateOnly;

ASTContext* context;

class RecursiveASTVisitorForSourceCodeInvastigator : public RecursiveASTVisitor<RecursiveASTVisitorForSourceCodeInvastigator>{
public:
    explicit RecursiveASTVisitorForSourceCodeInvastigator(Rewriter &r) : myRewriter(r) {}

    bool VisitStmt(Stmt *s){
        // Determine if statement is in the target file
        std::string sFileNamePath =  myRewriter.getSourceMgr().getFilename(s->getBeginLoc()).str();
        if (sFileNamePath !=currentFileName) {
            return true;
        }
        if (MuTeCUtils::isDebugMode()){
           
            std::cout << "Visit statement " <<  myRewriter.getSourceMgr().getFilename(s->getBeginLoc()).str() << "\n";
        }
        if (isa<BinaryOperator>(s) || isa<UnaryOperator>(s)){
            SourceLocation rewrittenCodeStart= myRewriter.getSourceMgr().getFileLoc(s->getBeginLoc());
            SourceLocation rewrittenCodeEnd = myRewriter.getSourceMgr().getFileLoc(s->getEndLoc());
            SourceRange rewrittenCodeRange;
            rewrittenCodeRange.setBegin(rewrittenCodeStart);
            rewrittenCodeRange.setEnd(rewrittenCodeEnd);

            std::string rewrittenCode = myRewriter.getRewrittenText(rewrittenCodeRange);

            if (rewrittenCode.find("${operator")!=std::string::npos){
                return true;
            }

            /*
            const Stmt* currentStmt = s;
            auto parents = context->getParents(*currentStmt);
            while(!parents.empty()){
                if (!parents[0].get<Stmt>()) break;
                if (isa<CallExpr>(parents[0].get<Stmt>())){
                    const CallExpr* functionCall = cast<CallExpr>(parents[0].get<Stmt>());
                    std::string functionName = myRewriter.getRewrittenText(functionCall->getCallee()->getSourceRange());
                    if (functionName == "barrier"){
                        return true;
                    }
                }
                currentStmt = parents[0].get<Stmt>();
                parents = context->getParents(*currentStmt);
            }
            */
        }

        if (isa<BinaryOperator>(s)){
            BinaryOperator* binaryOperator = cast<BinaryOperator>(s);
            std::string operatorStr = binaryOperator->getOpcodeStr().str() + "B";
            if (MuTeCUtils::isDebugMode()){
                    std::cout << "Binary operator " << binaryOperator->getOpcodeStr().str()  <<" found \n";
            }
            // special case for pointer operators
            if (isMutable(operatorStr)){
                // special case for pointer operations
                if (binaryOperator->getLHS()->getType()->isAnyPointerType() && binaryOperator->getRHS()->getType()->isAnyPointerType()){
                    if (operatorStr == "-B") {
                        return true;
                    }
                }

                if (notRewritable(myRewriter.getSourceMgr(), binaryOperator->getOperatorLoc())) return true;
                if (MuTeCUtils::isDebugMode()){
                    std::cout << "  - is rewritable\n";
                }
                /*
                SourceLocation startLoc = myRewriter.getSourceMgr().getFileLoc(
                    binaryOperator->getLocStart());
                SourceLocation endLoc = myRewriter.getSourceMgr().getFileLoc(
                    binaryOperator->getLocEnd());
                SourceRange newRange;
                newRange.setBegin(startLoc);
                newRange.setEnd(endLoc);

                std::cout << "[debug]" << myRewriter.getRewrittenText(newRange) << std::endl;
                */

                // special case for pointer operations
                if ((binaryOperator->getLHS()->getType()->isAnyPointerType() && binaryOperator->getRHS()->getType()->isIntegerType())
                    || (binaryOperator->getLHS()->getType()->isIntegerType() && binaryOperator->getRHS()->getType()->isAnyPointerType())){
                        if (operatorStr == "+B" || operatorStr == "-B"){
                            if (MuTeCUtils::isDebugMode()){
                                std::cout << "  - applied to pointer offset\n";
                            }
                            operatorStr = binaryOperator->getOpcodeStr().str() + "PB";
                        }
                }
                std::stringstream operatorTemplate;
                operatorTemplate << source_code_rewriter_constants::CODE_TEMPLATE_STR_PREFIX << currentOperator << "_" << operatorStr << "}";
                mutableOperatorTemplates[currentOperator] = operatorTemplate.str();
                if (MuTeCUtils::isDebugMode()){
                    std::cout << "  - replace by template for binary operator " << operatorStr << "\n";
                }
                myRewriter.ReplaceText(binaryOperator->getOperatorLoc(), operatorTemplate.str());
                if (MuTeCUtils::isDebugMode()){
                    std::cout << "  - done\n";
                }
                currentOperator++;
                numOperators++;
            }
        } else if (isa<UnaryOperator>(s)){
            UnaryOperator* unaryOperator = cast<UnaryOperator>(s);
            std::string operatorStr = unaryOperator->getOpcodeStr(unaryOperator->getOpcode()).str() + "U";
            if (MuTeCUtils::isDebugMode()){
                    std::cout << "Unary operator " << unaryOperator->getOpcodeStr(unaryOperator->getOpcode()).str()  <<" found \n";
            }
            if (isMutable(operatorStr)){
                if (notRewritable(myRewriter.getSourceMgr(), unaryOperator->getOperatorLoc())) return true;
                if (MuTeCUtils::isDebugMode()){
                    std::cout << "  - is rewritable\n";
                }
                /*
                SourceLocation startLoc = myRewriter.getSourceMgr().getFileLoc(
                    unaryOperator->getLocStart());
                SourceLocation endLoc = myRewriter.getSourceMgr().getFileLoc(
                    unaryOperator->getLocEnd());
                SourceRange newRange;
                newRange.setBegin(startLoc);
                newRange.setEnd(endLoc);

                std::cout << "[debug]" << myRewriter.getRewrittenText(newRange) << std::endl;
                */

                std::stringstream operatorTemplate;
                operatorTemplate << source_code_rewriter_constants::CODE_TEMPLATE_STR_PREFIX << currentOperator << "_" << operatorStr << "}";
                mutableOperatorTemplates[currentOperator] = operatorTemplate.str();
                if (MuTeCUtils::isDebugMode()){
                    std::cout << "  - replace by template for unary operator " << operatorStr << "\n";
                }
                myRewriter.ReplaceText(unaryOperator->getOperatorLoc(), operatorTemplate.str());
                if (MuTeCUtils::isDebugMode()){
                    std::cout << "  - done\n";
                }
                currentOperator++;
                numOperators++;
            }
        }
        return true;
    }

private:
    Rewriter &myRewriter;

    unsigned int getOperatorType(const std::string& operatorStr){
        if (operatorType.find(operatorStr) == operatorType.end()){
            return 0;
        } else {
            return operatorType[operatorStr];
        }
    }

    bool isMutable(const std::string& operatorStr){
        return (mutantOperator.find(operatorStr) == mutantOperator.end())? false: true;
    }

    bool notRewritable(const SourceManager& sm, const SourceLocation& sl){
        if (sm.isInExternCSystemHeader(sl)) return true;
        if (sm.isInSystemHeader(sl)) return true;
        if (sm.isMacroBodyExpansion(sl)) return true;
        if (sm.isMacroArgExpansion(sl)) return true;
        return false;
    }
};

class ASTConsumerForSourceCodeInvastigator : public ASTConsumer{
public:
    ASTConsumerForSourceCodeInvastigator(Rewriter &r): visitor(r) {}

    bool HandleTopLevelDecl(DeclGroupRef DR) override {
        if (MuTeCUtils::isDebugMode()){
                std::cout << "start running HandleTopLevelDecl\n";
            }
        for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
            // Traverse the declaration using our AST visitor.
            visitor.TraverseDecl(*b);
            //(*b)->dump();
        }
        return true;
    }

private:
    RecursiveASTVisitorForSourceCodeInvastigator visitor;
};

class ASTFrontendActionForSourceCodeInvastigator : public ASTFrontendAction {
public:
    ASTFrontendActionForSourceCodeInvastigator() {}

    void EndSourceFileAction() override {
        if (MuTeCUtils::isDebugMode()){
            std::cout << "start running EndSourceFileAction\n";
        }
        std::stringstream notification;
        const RewriteBuffer *buffer = myRewriter.getRewriteBufferFor(myRewriter.getSourceMgr().getMainFileID());
        if (buffer == NULL){
            llvm::outs() << "Rewriter buffer is null. Cannot write in file.\n";
            std::list<std::string> mutants;
            generatedMutantList[currentFileName] = mutants;
            currentOperator = 1;
            currentSourceFileID++;
            mutableOperatorTemplates.clear();
            return;
        }
        std::string rewriteBuffer = std::string(buffer->begin(), buffer->end());
        std::string source;
        std::string line;
        std::istringstream bufferStream(rewriteBuffer);
        while(getline(bufferStream, line)){
            source.append(line);
            source.append("\n");
        }
        
        std::string realCurrentFileName = currentFileName.substr(currentFileName.find_last_of('/'));
        
        std::string filename = directory + realCurrentFileName + "." + source_code_rewriter_constants::CODE_TEMPLATE_FILENAME_SUFFIX;
        std::ofstream outputFileStream(filename);
        outputFileStream << source;
        outputFileStream.close();
        UserConfig::removeFakeHeader(filename);
        
        std::list<std::string> mutants;
        if (!generateTemplateOnly){
            mutants = MuTeCUtils::generateMutant(filename, mutableOperatorTemplates, directory + realCurrentFileName, randomGenerate);
            notification << "|- " << mutants.size() << " mutants have been generated.";
            MuTeCUtils::alert(notification.str(), output_colour::KBLU);
        } else {
            notification << "|- code template have been generated.";
            MuTeCUtils::alert(notification.str(), output_colour::KBLU);
        }
        
        generatedMutantList[currentFileName] = mutants;
        currentOperator = 1;
        currentSourceFileID++;
        mutableOperatorTemplates.clear();
    }

    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &ci, 
        StringRef file) override {
            if (MuTeCUtils::isDebugMode()){
                std::cout << "start running CreateASTConsumer\n";
            }
            std::stringstream notification;
            notification << "[" << currentSourceFileID << "/" << numSourceFile << "] Instrumenting " << file.str();
            currentFileName = file.str();
            MuTeCUtils::alert(notification.str(), output_colour::KBLU);
            myRewriter.setSourceMgr(ci.getSourceManager(), ci.getLangOpts());
            context = &ci.getASTContext();
            return llvm::make_unique<ASTConsumerForSourceCodeInvastigator>(myRewriter);
        }

private:
    Rewriter myRewriter;
};

int parseCode(clang::tooling::ClangTool* tool, const int& numSourceFileIn, std::map<std::string, std::list<std::string>>** mutantFileList, const std::string& outputDirectory, bool random, bool templateOnly){
    if (MuTeCUtils::isDebugMode()){
        std::cout << "start parseCode\n";
    }
    codeTemplate = "";
    templateMap.clear();
    MuTeCUtils::initialiseOperatorTypeMap(operatorType);
    MuTeCUtils::initialiseMutantOperatorMap(mutantOperator);
    currentOperator = 1;
    numSourceFile = numSourceFileIn;
    currentSourceFileID = 1;
    numOperators=0;
    directory = outputDirectory;
    randomGenerate = random;
    generateTemplateOnly = templateOnly;

    if (MuTeCUtils::isDebugMode()){
        std::cout << "start running FrontendActionFactory\n";
    }
    tool->run(newFrontendActionFactory<ASTFrontendActionForSourceCodeInvastigator>().get());
    *mutantFileList = &generatedMutantList;
    return numOperators;
}