set(LLVM_LINK_COMPONENTS
    Support
)
        
add_clang_executable(mutec
    src/Utils.cpp
    src/Utils.h
    src/Constants.h
    src/SourceCodeInvastigator.cpp
    src/SourceCodeInvastigator.h
    src/UserConfig.cpp
    src/UserConfig.h
    src/Main.cpp)

add_clang_executable(mutec-template
    src/GeneratorForTemplate.cpp
    src/Utils.cpp
    src/Utils.h
    src/Constants.h
)
        
target_link_libraries(mutec PRIVATE
    clangAST
    clangASTMatchers
    clangBasic
    clangFrontend
    clangTooling)

target_link_libraries(mutec-template PRIVATE
    clangTooling)
