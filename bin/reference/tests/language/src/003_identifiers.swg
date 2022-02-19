// User identifiers start with '_' or an ascii letter.
// Identifiers can contain '_', an ascii letter or a digit number.

const thisIsAValidIdentifier0       = 0
const this_is_also_valid            = 0
const this_1_is_2_also__3_valid     = 0

// User identifiers cannot start with two underscores. This is reserved by the compiler.
// const __invalid = 0

/*

Some identifiers start with '#'. This indicates a compiler special keyword.

#assert
#run
#main
...

Some identifiers start with '@'. This indicates an intrinsic function.

@min()
@max()
@sqrt()
...

*/