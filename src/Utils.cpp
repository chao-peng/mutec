#include "Utils.h"
#include "Constants.h"

#include <iostream>
#include <list>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>

int MuTeCUtils::toInteger(std::string s){
    int power = 1;
    int result = 0;
    char c;
    for (int i = s.length() - 1; i >= 0; --i){
        c = s.at(i);
        if (!isdigit(c)) return 0;
        result = result + power * (c - '0');
        power *= 10;
    }
    return result;
}

bool MuTeCUtils::hasQuality(const unsigned int& quality, const unsigned int& qualityIdentifier){
    return ((quality & qualityIdentifier) == qualityIdentifier)? true: false;
}

void MuTeCUtils::initialiseOperatorTypeMap(std::map<std::string, unsigned int>& operatorTypeMap){
    operatorTypeMap["+B"]  = operator_type::ARITHMETIC;
    operatorTypeMap["-B"]  = operator_type::ARITHMETIC;
    operatorTypeMap["-U"]  = operator_type::ARITHMETIC;
    operatorTypeMap["*B"]   = operator_type::ARITHMETIC;
    operatorTypeMap["/B"]   = operator_type::ARITHMETIC;
    operatorTypeMap["%B"]   = operator_type::ARITHMETIC;
    operatorTypeMap["++U"]  = operator_type::ARITHMETIC;
    operatorTypeMap["--U"]  = operator_type::ARITHMETIC;
    operatorTypeMap["==B"]  = operator_type::RELATIONAL;
    operatorTypeMap["!=B"]  = operator_type::RELATIONAL;
    operatorTypeMap["<B"]   = operator_type::RELATIONAL;
    operatorTypeMap["<=B"]  = operator_type::RELATIONAL;
    operatorTypeMap[">B"]   = operator_type::RELATIONAL;
    operatorTypeMap[">=B"]  = operator_type::RELATIONAL;
    operatorTypeMap["&&B"]  = operator_type::LOGICAL;
    operatorTypeMap["||B"]  = operator_type::LOGICAL;
    operatorTypeMap["!U"]   = operator_type::LOGICAL;
    operatorTypeMap["&B"]   = operator_type::BITWISE;
    operatorTypeMap["|B"]   = operator_type::BITWISE;
    operatorTypeMap["^B"]   = operator_type::BITWISE;
    operatorTypeMap["~U"]   = operator_type::BITWISE;
    operatorTypeMap["<<B"]  = operator_type::BITWISE;
    operatorTypeMap[">>B"]  = operator_type::BITWISE;
    operatorTypeMap["=B"]   = operator_type::ASSIGNMENT;
    operatorTypeMap["+=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["-=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["*=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["/=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["%=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["<<=B"] = operator_type::ASSIGNMENT;
    operatorTypeMap[">>=B"] = operator_type::ASSIGNMENT;
    operatorTypeMap["&=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["|=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["^=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["&U"]   = operator_type::OTHER;
    operatorTypeMap["*U"]   = operator_type::OTHER;
}

void MuTeCUtils::initialiseMutantOperatorMap(std::map<std::string, std::list<std::string>>& mutantOperatorMap){
    //Arithmetic
    mutantOperatorMap["+B"] = {"-", "*", "/"};
    mutantOperatorMap["-B"] = {"+", "*", "/"};
    mutantOperatorMap["-U"] = {""};
    mutantOperatorMap["*B"] = {"/", "+", "-"};
    mutantOperatorMap["/B"] = {"*", "+", "-"};
    mutantOperatorMap["%B"] = {"*", "+", "-", "/"};
    mutantOperatorMap["++U"] = {"--"};
    mutantOperatorMap["--U"] = {"++"};
    //Relational
    mutantOperatorMap["<B"] = {">", ">=", "<="};
    mutantOperatorMap[">B"] = {"<", "<=", ">="};
    mutantOperatorMap["==B"] = {"!="};
    mutantOperatorMap["<=B"] = {">=", ">", "<"};
    mutantOperatorMap[">=B"] = {"<=", "<", ">"};
    mutantOperatorMap["!=B"] = {"=="};
    //Logical
    mutantOperatorMap["&&B"] = {"||"};
    mutantOperatorMap["||B"] = {"&&"};
    mutantOperatorMap["!U"] = {""};
    //Bitwise
    mutantOperatorMap["&B"] = {"|"};
    mutantOperatorMap["|B"] = {"&"};
    mutantOperatorMap["^B"] = {"&"};
    mutantOperatorMap["~U"] = {""};
    mutantOperatorMap["<<B"] = {">>"};
    mutantOperatorMap[">>B"] = {"<<"};
    //Assignment
    mutantOperatorMap["+=B"] = {"-=" ,"*=", "/="};
    mutantOperatorMap["-=B"] = {"+=", "*=", "/="};
    mutantOperatorMap["*=B"] = {"/=", "+=", "-="};
    mutantOperatorMap["/=B"] = {"*=", "+=", "-="};
    mutantOperatorMap["%=B"] = {"*=", "+=", "-=", "/="};
    mutantOperatorMap["<<=B"] = {">>="};
    mutantOperatorMap[">>=B"] = {"<<="};
    mutantOperatorMap["&=B"] = {"|="};
    mutantOperatorMap["|=B"] = {"&="};
    mutantOperatorMap["^=B"] = {"&="};
}

std::string MuTeCUtils::colourString(const std::string& str, const char* const colour){
    return colour + str + output_colour::KNRM;
}

void MuTeCUtils::resolveTemplate(const std::string& templateStr, int &id, std::string& operatorStr){
    int p1 = templateStr.find_first_of("_");
    int p2 = templateStr.find_last_of("_");
    int p3 = templateStr.find_last_of("}");
    std::string idStr = templateStr.substr(p1+1, p2-p1-1);
    operatorStr = templateStr.substr(p2+1, p3-p2-1);
    id = toInteger(idStr);
}

int MuTeCUtils::replaceStringPattern(std::string& originalString, const std::string& pattern, const std::string& value){
    auto locBegin = originalString.find(pattern);
    if (locBegin == originalString.npos){
        return error_code::STATUS_ERR;
    }
    originalString.replace(locBegin, pattern.length(), value);
    return error_code::STATUS_OK;
}

std::list<std::string> MuTeCUtils::generateMutant(const std::string& filename, std::map<int, std::string>& mutableOperatorTemplates, const std::string& filePath, bool random){
    std::ifstream sourceFile(filename);
    std::stringstream codeStream;
    std::string code;
    codeStream << sourceFile.rdbuf();
    code = codeStream.str();
    sourceFile.close();
    std::map<std::string, std::list<std::string>> mutantOperatorMap;
    initialiseMutantOperatorMap(mutantOperatorMap);
    int mutantID = 1;
    std::list<std::string> generatedMutants;
    
    if (random){
        int indexTargetTemplate = randomInInclusiveRange(1, mutableOperatorTemplates.size());
        std::string targetTemplate = mutableOperatorTemplates[indexTargetTemplate];
        std::string optFromTemplate; int ID;
        resolveTemplate(targetTemplate, ID, optFromTemplate);
        std::list<std::string> possibleOperators = mutantOperatorMap[optFromTemplate];
        auto ptrChosenOpt = possibleOperators.begin();
        std::advance(ptrChosenOpt, randomInInclusiveRange(0, possibleOperators.size()-1));
        std::cout << "Random generation info\n"
                << "Chosen template: " << targetTemplate << "\n"
                << "Opt in template & id: " << optFromTemplate << " " << ID << "\n"
                << "Chosen Replacement opt: " << *ptrChosenOpt << "\n"
                << "End of info\n";
        for (auto it = mutableOperatorTemplates.begin(); it!=mutableOperatorTemplates.end(); it++) {
            if (it->second == targetTemplate) {
                replaceStringPattern(code, targetTemplate, *ptrChosenOpt);
            } else {
                std::string currentTemplate = it->second;
                int currentID;
                std::string currentOperatorStr;
                resolveTemplate(currentTemplate, currentID, currentOperatorStr);
                currentOperatorStr = currentOperatorStr.substr(0, currentOperatorStr.length() -1);
                replaceStringPattern(code, currentTemplate, currentOperatorStr);
            }
        }
        std::stringstream fileNameBuilder;
        fileNameBuilder << filePath << "." << source_code_rewriter_constants::MUTANT_FILENAME_SUFFIX << mutantID;
        std::ofstream outputFile(fileNameBuilder.str());
        generatedMutants.push_back(fileNameBuilder.str());
        outputFile << code;
        outputFile.close();
        mutantID++;
    } else {
        for(auto it = mutableOperatorTemplates.begin(); it!=mutableOperatorTemplates.end(); it++){
            std::string tmpCode(code);
            std::string currentTemplate = it->second;
            int currentID;
            std::string currentOperatorStr;
            resolveTemplate(currentTemplate, currentID, currentOperatorStr);
            for (auto it2 = mutableOperatorTemplates.begin(); it2!=mutableOperatorTemplates.end(); it2++){
                if (it2->first != currentID){
                    std::string template2 = it2->second;
                    std::string operator2Str;
                    int operator2ID;
                    resolveTemplate(template2, operator2ID, operator2Str);
                    operator2Str = operator2Str.substr(0, operator2Str.length()-1);
                    replaceStringPattern(tmpCode, template2, operator2Str);
                }
            }
            std::list<std::string> mutantOperators = mutantOperatorMap[currentOperatorStr];
            for (auto it3 = mutantOperators.begin(); it3!=mutantOperators.end(); it3++){
                std::string codeToWrite(tmpCode);
                replaceStringPattern(codeToWrite, currentTemplate, *it3);
                std::stringstream fileNameBuilder;
                fileNameBuilder << filePath << "." << source_code_rewriter_constants::MUTANT_FILENAME_SUFFIX << mutantID;
                std::ofstream outputFile(fileNameBuilder.str());
                generatedMutants.push_back(fileNameBuilder.str());
                outputFile << codeToWrite;
                outputFile.close();
                mutantID++;
            }
        }
    }

    mutantID--;
    return generatedMutants;
}

int MuTeCUtils::runCommand(const std::string& command){
    return system(command.c_str());
}

int MuTeCUtils::alert(const std::string& message, const char* const colour, const int& msg_type){
    switch(msg_type){
        case (message_type::STD):
            std::cout << colourString(message, colour) << std::endl;
            return error_code::STATUS_OK;
        case (message_type::ERR):
            std::cerr << colourString(message, colour) << std::endl;
            return error_code::STATUS_OK;
        case (message_type::LOG):
            std::cout << colourString(message, colour) << std::endl;
            return error_code::STATUS_OK;
        default:
            return error_code::STATUS_ERR;
    }
}

int MuTeCUtils::randomInInclusiveRange(int min, int max){
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(min, max);
    return distr(eng);
}

std::list<size_t> MuTeCUtils::strFindAll(const std::string str, const std::string substr){
    std::list<size_t> result;
    if (!(str.length()||substr.length())) return result;

    size_t subLen = substr.length();
    size_t strLen = str.length();

    for (size_t i = 0; i <= (strLen - subLen);){
        std::string sstr = str.substr(i, subLen);
        if (sstr == substr){
            result.push_back(i);
            i = i + subLen;
            continue;
        } else {
            i++;
        }
    }
    
    return result;
}

std::list<size_t> MuTeCUtils::strFindAll(const std::string str, const char* const substr){
    return strFindAll(str, std::string(substr));
}

std::map<int, std::string> MuTeCUtils::retrieveTemplatesFromFile(const std::string& filename){
    size_t i;
    char ch;
    std::map<int, std::string> result;
    std::ifstream sourceFile(filename);
    std::stringstream codeStream;
    std::string code;
    codeStream << sourceFile.rdbuf();
    code = codeStream.str();
    sourceFile.close();
    std::map<std::string, unsigned int> operatorType;
    MuTeCUtils::initialiseOperatorTypeMap(operatorType);

    std::list<size_t> resultCandidatePos = strFindAll(code, source_code_rewriter_constants::CODE_TEMPLATE_STR_PREFIX);
    for (auto it = resultCandidatePos.begin(); it != resultCandidatePos.end();){
        int begin = *it;
        int end;
        it++;
        if (it == resultCandidatePos.end()){
            end = code.length();
        } else {
            end = *it;
        }

        std::string target = code.substr(begin, end - begin);
        target = target.substr(0, target.find_first_of('}') + 1);
        if (target == "") continue;

        int IDBegin = target.find_first_of('_') + 1;
        if (!isdigit(target[IDBegin])) continue;
        int IDLen = 1;
        for (i = IDBegin + 1; i < target.length(); i++ ){
            ch = target[i];
            if (isdigit(ch)){
                IDLen++;
            } else if (ch == '_') {
                break;
            } else {
                IDLen = -1;
            }
        }
        if (IDLen == -1) continue;

        int ID = toInteger(target.substr(IDBegin, IDLen));

        std::string OptStr = target.substr(target.find_last_of('_') + 1);
        if (OptStr.length() < 3 || OptStr.length() > 4) continue;
        OptStr = OptStr.substr(0, OptStr.length()-1);
        if (operatorType.find(OptStr) == operatorType.end()) continue;
        result[ID] = target;
    }
    return result;
}

std::string MuTeCUtils::getEnvVar(const std::string& var){
    const char* val = std::getenv(var.c_str());
    if (val == NULL) {
        return "";
    }
    else {
        return std::string(val);
    }
}

bool MuTeCUtils::isDebugMode(){
    std::string var = std::string(source_code_rewriter_constants::MUTEC_DEBUG_ENV_VAR);
    return getEnvVar(var) != "";
}

std::string MuTeCUtils::getFilenameFromPath(const std::string& path){
    if (path.at(path.length()-1)=='/') return "";
    size_t loc = path.find_last_not_of('/');
    std::string result = path.substr(0, loc + 1);
    loc = result.find_last_of('/');
    if (loc == std::string::npos) return result;
    else return result.substr(loc+1);
}
