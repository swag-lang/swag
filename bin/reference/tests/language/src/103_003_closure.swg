/**
Swag supports a limited implementation of the `closure` concept.

Currently, only a specific amount of capture (up to 48 bytes) is allowed, ensuring there are no hidden allocations. Additionally, you can only capture 'simple' variables—no structs with custom behaviors like `opDrop`, `opPostCopy`, or `opPostMove`.
*/

/**
A closure is declared similarly to a lambda, with the captured variables specified between `|...|` before the function parameters.
*/
#test
{
    let a = 125
    let b = 521

    // Capture 'a' and 'b' by value, meaning copies of these variables are captured.
    let fct: closure() = closure|a, b|()
    {
        // Inside the closure, the captured values 'a' and 'b' are accessible.
        @assert(a == 125)
        @assert(b == 521)
    }

    // Invoke the closure.
    fct()
}

/**
Variables can also be captured by reference using `&` (by default, they are captured by value).
*/
#test
{
    var a = 125

    // Capture 'a' by reference, meaning changes to 'a' inside the closure will affect 'a' outside the closure as well.
    let fct: closure() = closure|&a|()
    {
        // Modify the captured variable 'a' by incrementing it.
        a += 1
    }

    // Invoke the closure, which increments 'a' by 1.
    fct()
    @assert(a == 126)  // Check that 'a' has been incremented to 126.

    // Invoke the closure again, further incrementing 'a'.
    fct()
    @assert(a == 127)  // Check that 'a' has been incremented to 127.
}

/**
A closure variable can also hold a standard lambda (without capture).
*/
#test
{
    var fct: closure(s32, s32) -> s32

    // Assign a simple lambda that adds two integers to the closure variable 'fct'.
    fct = func(x, y) => x + y

    // Test the lambda by passing values 1 and 2, expecting the result to be 3.
    @assert(fct(1, 2) == 3)
}

/**
You can capture arrays, structs, slices, etc., as long as they fit within the maximum capture size and the struct is a Plain Old Data (POD) type.
*/
#test
{
    var x = [1, 2, 3]  // An array of integers.

    var fct: closure(s32) -> s32

    // Capture the array 'x' by value (a copy of the array is made).
    fct = closure|x|(toAdd)
    {
        var res = 0
        // Iterate over the captured array 'x' and sum its elements.
        visit v in x:
            res += v
        // Add the 'toAdd' parameter to the sum.
        res += toAdd
        return res
    }

    // Invoke the closure with the value 4, expecting the result to be the sum of 1+2+3+4.
    let result = fct(4)
    @assert(result == 1 + 2 + 3 + 4)
}

/**
Captured variables are mutable and part of the closure, allowing you to modify them.
*/
#test
{
    // A function that returns a closure, which increments a captured variable 'x' each time it's called.
    func getInc() -> closure() -> s32
    {
        let x = 10  // Initialize 'x' with 10.

        // Return a closure that captures 'x' by value.
        return closure|x|() -> s32
        {
            // Increment the captured 'x' and return its new value.
            x += 1
            return x
        }
    }

    // Obtain the closure returned by 'getInc'.
    let fct = getInc()

    // Invoke the closure multiple times, verifying that 'x' increments correctly.
    @assert(fct() == 11)  // First call, 'x' becomes 11.
    @assert(fct() == 12)  // Second call, 'x' becomes 12.
    @assert(fct() == 13)  // Third call, 'x' becomes 13.
}
