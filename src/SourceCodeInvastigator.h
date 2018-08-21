#ifndef MUTEC_SOURCE_CODE_INVASTIGATOR_HEADER_
#define MUTEC_SOURCE_CODE_INVASTIGATOR_HEADER_

#include "clang/Tooling/Tooling.h"
#include "UserConfig.h"

int parseCode(clang::tooling::ClangTool* tool, const int& numSourceFileIn, std::map<std::string, std::list<std::string>>** mutantFileList, const std::string& outputDirectory, bool random, bool templateOnly);

#endif