/**
# Introduction to Lambdas in Swag
A lambda in Swag is a **pointer to a function**. This allows functions to be stored in variables,
passed as arguments, or returned from other functions, making them versatile in functional programming.
*/
#test
{
    func myFunction0() {}
    func myFunction1(x: s32) => x * x

    // 'ptr0' is a pointer to a function that takes no parameters and returns nothing.
    let ptr0: func() = &myFunction0
    ptr0() // Call the function through the pointer

    // The type of 'ptr1' is inferred from 'myFunction1'.
    let ptr1 = &myFunction1
    @assert(myFunction1(2) == 4)
    @assert(ptr1(2) == 4) // Call the function using the pointer
}

/**
# Null Lambdas
A lambda can also be null, indicating that it does not point to any function. This is useful for
optional callbacks or deferred initialization of function pointers.
*/
#test
{
    var lambda: func()->bool
    @assert(lambda == null) // Confirm that the lambda is initially null
}

/**
# Using Lambdas as Function Parameters
Lambdas can be passed as parameters, enabling higher-order functions where other functions
are executed dynamically based on input.
*/
#test
{
    alias Callback = func(s32)->s32
    func toDo(value: s32, ptr: Callback)->s32 => ptr(value) // Execute the lambda with the given value

    func square(x: s32) => x * x
    @assert(toDo(4, &square) == 16) // Pass the square function as a callback
}

/**
# Anonymous Functions
Anonymous functions, or function literals, can be defined directly in your code without the need for
a named identifier, making them convenient for quick, inline functionality.
*/
#test
{
    var cb = func(x: s32)->s32 => x * x // Define an anonymous function that squares a number
    @assert(cb(4) == 16)

    cb = func(x: s32)->s32 => x * x * x // Reassign to an anonymous function that cubes a number
    @assert(cb(4) == 64)
}

/**
# Passing Anonymous Functions as Parameters
Anonymous functions can be passed directly as arguments to other functions, without first being
assigned to variables. This enables concise and flexible code.
*/
#test
{
    alias Callback = func(s32)->s32
    func toDo(value: s32, ptr: Callback)->s32 => ptr(value)

    @assert(toDo(4, func(x: s32) => x * x) == 16) // Passing anonymous functions as arguments
    @assert(toDo(4, func(x: s32) => x + x) == 8)
    @assert(toDo(4, func(x: s32)->s32 { return x - x; }) == 0)
}

/**
# Inferred Parameter Types in Anonymous Functions
Swag allows for inferred parameter types in anonymous functions, resulting in cleaner and more
concise code, especially when the type can be unambiguously deduced from context.
*/
#test
{
    alias Callback = func(s32)->s32
    func toDo(value: s32, ptr: Callback)->s32 => ptr(value)

    @assert(toDo(4, func(x) => x * x) == 16) // The type of 'x' is inferred from context
    @assert(toDo(4, func(x) => x + x) == 8)
    @assert(toDo(4, func(x) { return x - x; }) == 0)
}

/**
# Omitting Types When Assigning Lambdas
When assigning a lambda to a variable, parameter and return types can be omitted if they are
inferable from the variable's type. This reduces verbosity while maintaining type safety.
*/
#test
{
    var fct: func(s32, s32)->bool

    fct = func(x, y) => x == y // Assign a lambda with inferred parameter types
    @assert(fct(10, 10))

    // Assign a lambda with a block body
    fct = func(x, y)
    {
        return x != y
    }

    @assert(fct(20, 120))
}

/**
# Lambdas with Default Parameter Values
Lambdas in Swag can have default parameter values, enhancing their flexibility and allowing them
to adapt to various contexts with minimal adjustments.
*/
#test
{
    {
        let x = func(val = true)
        {
            @assert(val == true)
        }

        x() // Call the lambda without arguments
    }

    {
        var x: func(val: bool = true)
        x = func(val)
        {
            @assert(val == true)
        }

        x() // Call with the default value
        x(true) // Explicitly pass the value
    }
}
