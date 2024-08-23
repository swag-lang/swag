/**
# Naming
User-defined identifiers, such as variables, constants, and function names, must begin with either an underscore or an ASCII letter. These identifiers can then include underscores, ASCII letters, and digits.

Examples:
*/

const thisIsAValidIdentifier0   = 0
const this_is_also_valid        = 0
const this_1_is_2_also__3_valid = 0

/**
However, identifiers cannot start with two underscores, as this prefix is reserved by the compiler.
*/

// const __this_is_invalid = 0

/**
# Compiler Instructions
Additionally, some identifiers may begin with `#`, indicating a compiler instruction. These instructions have specific roles within the Swag programming environment.

Examples of compiler instructions:
```swag
#assert
#run
#main
```

# Intrinsics
Identifiers that start with '@' represent intrinsic functions. These functions may be available only at compile time or at both compile time and runtime, depending on the specific intrinsic.

Examples of intrinsic functions:
```swag

@min()
@max()
@sqrt()
@print()
```
*/
