/**
# Top-Level Declaration Order

The order of all **top-level** declarations in Swag does not matter. This means
you can reference constants, variables, or functions before they are defined —
either within the same file or across multiple files.

This flexibility is especially useful in large codebases, where logical flow or
readability may benefit from organizing code independently of declaration order.
*/

// In this example, we declare a constant 'A' and initialize it with 'B',
// even though 'B' has not yet been declared or defined.
const A = B

// Next, we declare a constant 'B' and initialize it with 'C',
// which is also not yet declared or defined.
const B = C

// Finally, we declare and define 'C' as a constant of type 'u64'
// (an unsigned 64-bit integer) with a value of 1.
// This retroactively assigns values to both 'A' and 'B'
// based on the earlier assignments.
const C: u64 = 1

/**
In this example, we demonstrate Swag's flexibility by calling the function
'functionDeclaredLater' before it is defined. This behavior illustrates that
Swag does not impose order restrictions on function declarations.
*/
#run
{
    // Call the function 'functionDeclaredLater' before it is declared.
    // Swag allows this because top-level declarations are order-independent.
    functionDeclaredLater()
}

// The function is declared here, after it has already been called.
func functionDeclaredLater() {}

/**
This flexibility also applies across multiple files. For example, you can call a
function in one file and define it in another. Swag's global declaration model
is intentionally non-restrictive, allowing you to structure your code in the way
that best supports clarity and maintainability.
*/
