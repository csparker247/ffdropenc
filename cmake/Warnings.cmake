# Also enable warnings project-wide
set(project_warnings
    # Group warnings
    -Wall
    -Wextra
    -Weffc++
    -pedantic

    # Extra warnings we want to enable
    -Wambiguous-ellipsis
    -Warray-bounds-pointer-arithmetic
    -Wassign-enum
    -Wattributes
    -Wbad-function-cast
    -Wbind-to-temporary-copy
    -Wbitfield-constant-conversion
    -Wbitwise-op-parentheses
    -Wbool-conversion
    -Wc++14-compat
    -Wc++11-long-long
    -Wcast-align
    -Wcast-qual
    -Wchar-subscripts
    -Wclass-varargs
    #-Wcomma
    -Wcomment
    -Wconditional-uninitialized
    -Wconversion                    # Disable ones you don't want
    -Wdelete-incomplete
    -Wdelete-non-virtual-dtor
    -Wdeprecated-implementations
    -Wdirect-ivar-access
    -Wdivision-by-zero
    -Wdocumentation
    -Wduplicate-enum
    -Wduplicate-method-arg
    -Wduplicate-method-match
    -Wenum-compare
    -Wenum-conversion
    -Wexplicit-ownership-type
    -Wextra-semi
    -Wextra-tokens
    #-Wfloat-overflow-conversion
    -Wheader-guard
    -Wheader-hygiene
    -Widiomatic-parentheses
    -Wimplicit
    -Wimplicit-fallthrough
    -Winfinite-recursion
    -Wlocal-type-template-args
    -Wloop-analysis
    -Wmain
    -Wmethod-signatures
    -Wmismatched-tags
    -Wmissing-field-initializers
    -Wmissing-noreturn
    -Wmissing-prototypes
    -Wmove
    -Wnew-returns-null
    #-Wnonportable-system-include-path
    -Wold-style-cast
    -Woverloaded-virtual
    -Wpessimizing-move
    -Wpointer-arith
    -Wredundant-move
    -Wself-assign
    -Wself-move
    -Wsemicolon-before-method-body
    #-Wshadow-all
    -Wsometimes-uninitialized
    -Wstring-conversion
    -Wstrlcpy-strlcat-size
    -Wstrncat-size
    -Wtautological-compare
    -Wthread-safety-analysis
    -Wundef
    -Wuninitialized
    -Wunneeded-internal-declaration
    -Wunreachable-code
    -Wunreachable-code-break
    -Wunreachable-code-return
    -Wunused
    -Wunused-macros
    -Wunused-member-function
    -Wused-but-marked-unused
    -Wvexing-parse
    -Wvla

    # Disable noisy or unncessary warnings
    -Wno-c++11-narrowing
    -Wno-c++1z-extensions
    -Wno-c++98-compat
    -Wno-missing-braces
    -Wno-switch-enum
    -Wno-sign-conversion
    -Wno-shorten-64-to-32
    -Wno-unknown-attributes
    -Wno-unused-function
)
add_compile_options(${project_warnings})