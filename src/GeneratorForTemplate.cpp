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
    llvm::cl::Optional
);

static llvm::cl::opt<bool> randomGenerate (
    "r",
    llvm::cl::desc("Randomly genenrate 1 mutation for each provided source file"),
    llvm::cl::value_desc("boolean"),
    llvm::cl::Optional
);

static llvm::cl::opt<bool> describeTemplate(
    "d",
    llvm::cl::desc("Describe the template"),
    llvm::cl::value_desc("boolean"),
    llvm::cl::Optional
);

int main(int argc, const char** argv){
    clang::tooling::CommonOptionsParser optionsParser(argc, argv, ToolCategory);

    if (!describeTemplate && outputDirectory == ""){
        std::string notification = "Please provide output directory to store mutants or use -d to see details of the template";
        MuTeCUtils::alert(notification, output_colour::KRED, message_type::ERR);
        return 0;
    }

    std::string dir(outputDirectory);
    if (outputDirectory != ""){
        std::string dir(outputDirectory);
        if (dir.at(dir.length()-1) != '/') {
            dir = dir + "/";
        }
    }
    
    std::string filename;
    for (auto it = optionsParser.getSourcePathList().begin(); it!=optionsParser.getSourcePathList().end();it++){
        filename = it->c_str();
        std::map<int, std::string> templates = MuTeCUtils::retrieveTemplatesFromFile(filename);
        std::stringstream notification;
        notification << filename;
        MuTeCUtils::alert(notification.str(), output_colour::KBLU);
        if (describeTemplate) {
            notification.str("");
            notification << "|- " << templates.size() << " operators found.";
            MuTeCUtils::alert(notification.str(), output_colour::KBLU);
        }
        if (outputDirectory != ""){
            std::list<std::string> mutants = MuTeCUtils::generateMutant(filename, templates, dir + filename, randomGenerate);
            notification.str("");
            notification << "|- " << mutants.size() << " mutants have been generated.";
            MuTeCUtils::alert(notification.str(), output_colour::KBLU);
        }
    }
    return 0;
}