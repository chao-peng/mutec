#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <map>

#include "Utils.h"
#include "Constants.h"
#include "SourceCodeInvastigator.h"

#include "llvm/Support/CommandLine.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

static llvm::cl::OptionCategory ToolCategory("MuTeC options");

/*
static llvm::cl::opt<std::string> executableFile(
    "run",
    llvm::cl::desc("Specify the executable file"),
    llvm::cl::value_desc("executable"),
    llvm::cl::Optional
);

static llvm::cl::opt<std::string> expectedOutput(
    "compare",
    llvm::cl::desc("Specify the expected output file path"),
    llvm::cl::value_desc("filename"),
    llvm::cl::Optional
);

static llvm::cl::opt<std::string> keepMutants(
    "keep",
    llvm::cl::desc("Keep which kind (killed, survived, all, none) of generated mutants after execution."),
    llvm::cl::value_desc("type string"),
    llvm::cl::Optional
);

static llvm::cl::opt<std::string> matchLines(
    "lines",
    llvm::cl::desc("Compare certain lines of output with the expected output file"),
    llvm::cl::value_desc("number of lines"),
    llvm::cl::Optional
);
*/

static llvm::cl::opt<std::string> userConfigFileName(
    "config",
    llvm::cl::desc("Specify the user config file name"),
    llvm::cl::value_desc("filename"),
    llvm::cl::Optional // Will be empty string if not specified
);

static llvm::cl::opt<std::string> outputDirectory(
    "o",
    llvm::cl::desc("Specify the directory for generated mutants"),
    llvm::cl::value_desc("directory"),
    llvm::cl::Required
);

/*
static llvm::cl::opt<std::string> userSpecifiedTimeout(
    "timeout",
    llvm::cl::desc("Specify the timeout of execution"),
    llvm::cl::value_desc("seconds"),
    llvm::cl::Optional // Will be empty string if not specified
);
*/
int main(int argc, const char** argv){
    std::stringstream notification;
    clang::tooling::CommonOptionsParser optionsParser(argc, argv, ToolCategory);

    // Set of Source code files
    std::list<std::string> lstSourceFile;
    
    std::string directory(outputDirectory.c_str());
    if (directory.at(directory.length()-1) != '/') directory = directory + "/";

    /*
    // Expected output file
    std::string expectedOutputFilePath;
    // Executable file
    std::string executableFilePath;
    // Lines in the expected output file to match
    std::list<int> lines;

    std::list<std::string> survivedMutants;
    */

    std::string filename;
    for (auto it = optionsParser.getSourcePathList().begin(); it!=optionsParser.getSourcePathList().end();it++){
        filename = it->c_str();
        lstSourceFile.push_back(filename);
    }

    int numSourceFile = lstSourceFile.size();
    if (numSourceFile == 0){
        MuTeCUtils::alert("Please provide at least one source code to process.", output_colour::KRED, message_type::ERR);
        exit(error_code::NO_SOURCE_FILE_SUPPLIED);
    }

    UserConfig userConfig(userConfigFileName.c_str());
    if (!userConfig.isEmpty()){
        for (auto itSourceFile = lstSourceFile.begin(); itSourceFile != lstSourceFile.end(); itSourceFile++){
            userConfig.generateFakeHeader(*itSourceFile);
        }
    }
   
    clang::tooling::ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());
    std::map<std::string, std::list<std::string>>* mutantFileList;
    int numOperators = parseCode(&tool, numSourceFile, &mutantFileList, directory);

    if (!userConfig.isEmpty()){
        for (auto itSourceFile = lstSourceFile.begin(); itSourceFile != lstSourceFile.end(); itSourceFile++){
            UserConfig::removeFakeHeader(*itSourceFile);
        }
    }

    notification << "Code invastigation summary: " << numOperators << " mutable operators in total have been found.";
    MuTeCUtils::alert(notification.str(), output_colour::KBLU);
    notification.str("");    

}