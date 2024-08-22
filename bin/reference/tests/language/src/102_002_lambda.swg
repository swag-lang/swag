/**
# Introduction to Lambdas in Swag
A lambda in Swag is essentially a **pointer to a function**. This allows functions to be stored in variables, passed as arguments, or returned from other functions.
*/
#test
{
    func myFunction0() {}
    func myFunction1(x: s32) => x * x

    // 'ptr0' is a pointer to a function that takes no parameters and returns nothing.
    let ptr0: func() = &myFunction0
    ptr0()  // Call the function through the pointer

    // The type of 'ptr1' is inferred from 'myFunction1'.
    let ptr1 = &myFunction1
    @assert(myFunction1(2) == 4)
    @assert(ptr1(2) == 4)  // Call the function using the pointer
}

/**
# Null Lambdas
A lambda can also be null, which means it does not point to any function.
*/
#test
{
    var lambda: func() -> bool
    @assert(lambda == null)  // Confirm that the lambda is initially null
}

/**
# Using Lambdas as Function Parameters
Lambdas can be used as function parameter types, allowing functions to take other functions as arguments.
*/
#test
{
    typealias Callback = func(s32) -> s32
    func toDo(value: s32, ptr: Callback) -> s32 => ptr(value)  // Execute the lambda with the given value

    func square(x: s32) => x * x
    @assert(toDo(4, &square) == 16)  // Pass the square function as a callback
}

/**
# Anonymous Functions
You can also create *anonymous functions* (also known as function literals) directly in your code.
*/
#test
{
    var cb = func(x: s32) -> s32 => x * x  // Define an anonymous function that squares a number
    @assert(cb(4) == 16)
    
    // Reassign the variable to a new anonymous function that cubes a number
    cb = func(x: s32) -> s32 => x * x * x
    @assert(cb(4) == 64)
}

/**
# Passing Anonymous Functions as Parameters
Anonymous functions can be passed directly as parameters to other functions without needing to be assigned to a variable first.
*/
#test
{
    typealias Callback = func(s32) -> s32
    func toDo(value: s32, ptr: Callback) -> s32 => ptr(value)

    // Passing anonymous functions as arguments
    @assert(toDo(4, func(x: s32) => x * x) == 16)
    @assert(toDo(4, func(x: s32) => x + x) == 8)
    @assert(toDo(4, func(x: s32) -> s32 { return x - x; }) == 0)
}

/**
# Inferred Parameter Types in Anonymous Functions
Parameter types in anonymous functions can be inferred, allowing for cleaner and more concise code.
*/
#test
{
    typealias Callback = func(s32) -> s32
    func toDo(value: s32, ptr: Callback) -> s32 => ptr(value)

    // The types of 'x' are inferred from the context
    @assert(toDo(4, func(x) => x * x) == 16)
    @assert(toDo(4, func(x) => x + x) == 8)
    @assert(toDo(4, func(x) { return x - x; }) == 0)
}

/**
# Omitting Types When Assigning Lambdas
When assigning a lambda to a variable, the types of parameters and the return type can also be omitted, as they will be inferred from the variable's type.
*/
#test
{
    var fct: func(s32, s32) -> bool

    // Assign a lambda with inferred parameter types
    fct = func(x, y) => x == y
    @assert(fct(10, 10))

    // Assign a lambda with a block body
    fct = func(x, y) { return x != y; }
    @assert(fct(20, 120))
}

/**
# Lambdas with Default Parameter Values
Lambdas can have default parameter values, making them flexible and easy to use in various contexts.
*/
#test
{
    {
        // Lambda with a default parameter value
        let x = func(val = true)
        {
            @assert(val == true)
        }

        x()  // Call the lambda without arguments, using the default value
    }

    {
        var x: func(val: bool = true)

        // Lambda where the parameter type is inferred, and the default value is used
        x = func(val)
        {
            @assert(val == true)
        }

        x()       // Call with the default value
        x(true)   // Explicitly pass the value
    }
}
