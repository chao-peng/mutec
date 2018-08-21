#ifndef MUTEC_CONSTANTS_HEADER_
#define MUTEC_CONSTANTS_HEADER_

namespace source_code_rewriter_constants{
    const char* const FAKE_HEADER_MACRO = "MUTEC_FAKE_HEADER_FOR_LIBTOOLING_";
    const char* const CODE_TEMPLATE_FILENAME_SUFFIX = "code_template";
    const char* const MUTANT_FILENAME_SUFFIX = "mutec";
    const char* const CODE_TEMPLATE_STR_PREFIX = "${operator_";
    const int CODE_TEMPLATE_STR_PREFIX_LEN = 11;
    const char* const MUTEC_DEBUG_ENV_VAR = "mutec_debug";
}

namespace error_code{
    const int STATUS_OK = 0;
    const int STATUS_ERR = 1;
    const int NO_SOURCE_FILE_SUPPLIED = 2;
    const int NO_NEED_TO_GENERATE_MUTANTS = 3;
    const int FAKE_HEADER_ALREADY_EXISTS = 4;
    const int NO_EXISTING_FAKE_HEADER = 5;
    const int LINES_TO_COMPARE_NOT_VALID = 7;
    const int COMMAND_EXEC_ERROR = 8;
    const int COMPARISON_FILE_NOT_PROVIDED = 9;
}

namespace operator_type{
    const unsigned int RESERVED_KEY0 = 0x0001;
    const unsigned int RESERVED_KEY1 = 0x0002;
    const unsigned int ARITHMETIC = 0x0004;
    const unsigned int RELATIONAL = 0x0008;
    const unsigned int LOGICAL = 0x0010;
    const unsigned int BITWISE = 0x0020;
    const unsigned int ASSIGNMENT = 0x0040;
    const unsigned int OTHER = 0x0080;
}

namespace output_colour{
    const char* const KNRM = "\x1B[0m";
    const char* const KRED = "\x1B[31m";
    const char* const KGRN = "\x1B[32m";
    const char* const KYEL = "\x1B[33m";
    const char* const KBLU = "\x1B[34m";
    const char* const KMAG = "\x1B[35m";
    const char* const KCYN = "\x1B[36m";
    const char* const KWHT = "\x1B[37m";
}

namespace message_type{
    const unsigned int STD = 0x001;
    const unsigned int ERR = 0x002;
    const unsigned int LOG = 0x003;

}

namespace mutant_type{
    const unsigned int KILLED = 0x01;
    const unsigned int SURVIVED = 0x02;
}
#endif