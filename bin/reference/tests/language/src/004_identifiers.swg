/**
User identifiers (like variables, constants, function names...) must start with an underscore or an ascii letter. Those identifiers can then contain underscores, ascii letters and digit numbers.
*/

const thisIsAValidIdentifier0   = 0
const this_is_also_valid        = 0
const this_1_is_2_also__3_valid = 0

/**
But your identifiers cannot start with two underscores. This is reserved by the compiler.
*/

// const __this_is_invalid = 0

/**
Note that some identifiers may start with `#`. This indicates a **compiler special keyword**.
```
#assert
#run
#main
```

Some identifiers can also start with `@`. This indicates an **intrinsic** function which can be compile time only or also available at runtime (this depends on the intrinsic).
```
@min()
@max()
@sqrt()
@print()
```
*/