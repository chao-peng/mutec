#include <iostream>
#include <list>
#include <sstream>

#include "Utils.h"
#include "Constants.h"

#include "llvm/Support/CommandLine.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

static llvm::cl::OptionCategory ToolCategory("mutec-template options");

static llvm::cl::opt<std::string> outputDirectory(
    "o",
    llvm::cl::desc("Specify the directory for generated mutants"),
    llvm::cl::value_desc("directory"),
    llvm::cl::Required
);

static llvm::cl::opt<bool> randomGenerate (
    "r",
    llvm::cl::desc("Randomly genenrate 1 mutation for each provided source file"),
    llvm::cl::value_desc("boolean"),
    llvm::cl::Optional
);

int main(int argc, const char** argv){
    clang::tooling::CommonOptionsParser optionsParser(argc, argv, ToolCategory);

    std::string dir(outputDirectory);
    if (dir.at(dir.length()-1) != '/') {
        dir = dir + "/";
    }

    std::string filename;
    for (auto it = optionsParser.getSourcePathList().begin(); it!=optionsParser.getSourcePathList().end();it++){
        filename = it->c_str();
        std::map<int, std::string> templates = MuTeCUtils::retrieveTemplatesFromFile(filename);
        std::list<std::string> mutants = MuTeCUtils::generateMutant(filename, templates, dir + filename, randomGenerate);
        std::stringstream notification;
        notification << "|- " << mutants.size() << " mutants have been generated.";
        MuTeCUtils::alert(notification.str(), output_colour::KBLU);
    }
    return 0;
}