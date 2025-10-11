/**
# Introduction to Closures in Swag
Swag supports a limited implementation of the closure concept. A closure allows capturing variables
from its surrounding scope. Swag currently permits capturing up to 48 bytes, ensuring no hidden
allocations. However, only simple variables (i.e., variables without custom behaviors such as `opDrop`,
`opPostCopy`, or `opPostMove`) are eligible for capture.
*/

/**
# Declaring a Closure
A closure is declared similarly to a lambda, with captured variables specified between `|...|` before
the function parameters. For a type, the syntax is `func||(...)`.
*/
#test
{
    let a = 125     // Initialize variable 'a' with a value of 125.
    let b = 521     // Initialize variable 'b' with a value of 521.

    // Capture 'a' and 'b' by value, meaning copies of these variables are captured.
    let fct: func||() = func|a, b|()
    {
        // Inside the func, the captured values 'a' and 'b' are accessible.
        @assert(a == 125) // Verify that the captured 'a' equals 125.
        @assert(b == 521) // Verify that the captured 'b' equals 521.
    }

    fct() // Invoke the func.
}

/**
# Capturing Variables by Reference
Variables can also be captured by reference using `&`. By default, variables are captured by value.
*/
#test
{
    var a = 125 // Declare a mutable variable 'a' with an initial value of 125.

    // Capture 'a' by reference, meaning changes to 'a' inside the func will affect 'a' outside the func.
    let fct: func||() = func|&a|()
    {
        a += 1 // Modify the captured variable 'a' by incrementing it.
    }

    fct() // Invoke the func, which increments 'a' by 1.
    @assert(a == 126) // Check that 'a' has been incremented to 126.

    fct() // Invoke the func again, further incrementing 'a'.
    @assert(a == 127) // Check that 'a' has been incremented to 127.
}

/**
# Assigning Lambdas to Closure Variables
A closure variable can also hold a standard lambda (without capture). This provides flexibility in
assigning different types of functions to the same variable.
*/
#test
{
    var fct: func||(s32, s32)->s32 // Declare a func variable that takes two integers and returns an integer.

    // Assign a simple lambda that adds two integers to the func variable 'fct'.
    fct = func(x, y) => x + y

    @assert(fct(1, 2) == 3) // Test the lambda by passing values 1 and 2, expecting the result to be 3.
}

/**
# Capturing Complex Types
You can capture arrays, structs, slices, etc., as long as they fit within the maximum capture size and
the struct is a Plain Old Data (POD) type.
*/
#test
{
    var x = [1, 2, 3] // Declare and initialize an array 'x' of integers.

    var fct: func||(s32)->s32 // Declare a closure variable that takes an integer and returns an integer.

    // Capture the array 'x' by value (a copy of the array is made).
    fct = func|x|(toAdd)
    {
        var res = 0 // Initialize a result variable 'res' to accumulate the sum.
        foreach v in x do  // Iterate over the captured array 'x' and sum its elements.
            res += v
        res += toAdd // Add the 'toAdd' parameter to the sum.
        return res
    }

    let result = fct(4) // Invoke the closure with the value 4.
    @assert(result == 1 + 2 + 3 + 4) // Verify the result is the sum of 1 + 2 + 3 + 4.
}

/**
# Modifying Captured Variables
Captured variables are mutable and part of the closure, allowing you to modify them. This enables the
creation of stateful functions.
*/
#test
{
    // A function that returns a closure, which increments a captured variable 'x' each time it's called.
    func getInc()->func||()->s32
    {
        let x = 10 // Initialize 'x' with 10.

        // Return a closure that captures 'x' by value.
        return func|x|()->s32
        {
            x += 1 // Increment the captured 'x' and return its new value.
            return x
        }
    }

    let fct = getInc() // Obtain the closure returned by 'getInc'.

    @assert(fct() == 11) // First call, 'x' becomes 11.
    @assert(fct() == 12) // Second call, 'x' becomes 12.
    @assert(fct() == 13) // Third call, 'x' becomes 13.
}
