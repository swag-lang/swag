/**
The order of all **top-level** declarations in Swag does not matter. This means that you can reference constants, variables, or functions before they are defined within the same file or even across multiple files.
*/

// Here we declare a constant 'A' and initialize it with 'B', 
// even though 'B' has not been declared or defined yet.
const A = B

// Now we declare a constant 'B' and initialize it with 'C',
// which is also not yet declared or defined.
const B = C

// Finally, we declare and define 'C' as a constant of type 'u64' 
// (an unsigned 64-bit integer) with a value of 1.
// This retroactively defines 'A' and 'B' as well, based on the earlier assignments.
const C: u64 = 1

/**
In this test, we call the function `functionDeclaredLater` before it is defined. This is acceptable in Swag, demonstrating the flexibility of top-level declaration order.
*/

#run
{
    // Here, we call the function 'functionDeclaredLater' before it is declared.
    // This works fine because Swag allows functions to be called before their definition.
    functionDeclaredLater()
}

// The function is declared here, after it was called.
func functionDeclaredLater() {}

/**
Note that the order of declarations is not only irrelevant within the same file but also across multiple files. For example, you can call a function in one file and define it in another. The global order of declarations does not matter!
*/
