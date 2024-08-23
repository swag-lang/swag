/**
The order of all **top-level** declarations in Swag does not matter. This feature allows you to reference constants, variables, or functions before they are defined, either within the same file or even across multiple files. This flexibility can be particularly useful in large codebases, where the natural flow of logic or readability may benefit from organizing code independently of its declaration order.
*/

// In this example, we declare a constant 'A' and initialize it with 'B', 
// even though 'B' has not yet been declared or defined.
const A = B

// Next, we declare a constant 'B' and initialize it with 'C',
// which is also not declared or defined at this point.
const B = C

// Finally, we declare and define 'C' as a constant of type 'u64' 
// (an unsigned 64-bit integer) with a value of 1.
// This retroactively assigns values to both 'A' and 'B', 
// based on the earlier assignments.
const C: u64 = 1

/**
In this test, we demonstrate Swag's flexibility by calling the function `functionDeclaredLater` before it is defined. Swag's allowance for this behavior highlights the language's flexibility in handling the order of top-level declarations.
*/

#run
{
    // We call the function 'functionDeclaredLater' before it is declared.
    // Swag permits this because the order of function declarations and calls is flexible.
    functionDeclaredLater()
}

// The function is declared here, after it has already been called.
func functionDeclaredLater() {}

/**
It is important to note that this flexibility applies not only within the same file but also across multiple files. For instance, you can call a function in one file and define it in another. The global order of declarations in Swag is designed to be non-restrictive, allowing for a more intuitive organization of your code.
*/
