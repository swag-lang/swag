/**
# Introduction to Lambdas in Swag

A lambda in Swag is a pointer to a function. This allows functions to be stored in variables,
passed as arguments, or returned from other functions—enabling functional programming patterns.
*/
#test
{
    func myFunction0() {}
    func myFunction1(x: s32) => x * x

    let ptr0: func() = &myFunction0
    ptr0()

    let ptr1 = &myFunction1
    @assert(myFunction1(2) == 4)
    @assert(ptr1(2) == 4)
}

/**
# Null Lambdas

A lambda can be null, representing an uninitialized or absent function pointer.
This is useful for optional callbacks or deferred initialization.
*/
#test
{
    var lambda: func()->bool
    @assert(lambda == null)
}

/**
# Using Lambdas as Function Parameters

Lambdas can be passed as arguments, enabling higher-order functions that call
other functions dynamically.
*/
#test
{
    alias Callback = func(s32)->s32
    func toDo(value: s32, ptr: Callback)->s32 => ptr(value)

    func square(x: s32) => x * x
    @assert(toDo(4, &square) == 16)
}

/**
# Anonymous Functions

Anonymous (inline) functions can be defined without names for short, inline logic.
*/
#test
{
    var cb = func(x: s32)->s32 => x * x
    @assert(cb(4) == 16)

    cb = func(x: s32)->s32 => x * x * x
    @assert(cb(4) == 64)
}

/**
# Passing Anonymous Functions as Parameters

Anonymous functions can be passed directly as arguments.
*/
#test
{
    alias Callback = func(s32)->s32
    func toDo(value: s32, ptr: Callback)->s32 => ptr(value)

    @assert(toDo(4, func(x: s32) => x * x) == 16)
    @assert(toDo(4, func(x: s32) => x + x) == 8)
    @assert(toDo(4, func(x: s32)->s32 { return x - x; }) == 0)
}

/**
# Inferred Parameter Types in Anonymous Functions

If the type is clear from context, lambda parameters can omit explicit types.
*/
#test
{
    alias Callback = func(s32)->s32
    func toDo(value: s32, ptr: Callback)->s32 => ptr(value)

    @assert(toDo(4, func(x) => x * x) == 16)
    @assert(toDo(4, func(x) => x + x) == 8)
    @assert(toDo(4, func(x) { return x - x; }) == 0)
}

/**
# Omitting Types When Assigning Lambdas

When the variable type is known, parameter and return types can be omitted in the lambda.
*/
#test
{
    var fct: func(s32, s32)->bool

    fct = func(x, y) => x == y
    @assert(fct(10, 10))

    fct = func(x, y)
    {
        return x != y
    }

    @assert(fct(20, 120))
}

/**
# Lambdas with Default Parameter Values

Lambdas may have parameters with default values, allowing flexible invocation.
*/
#test
{
    {
        let x = func(val = true)
        {
            @assert(val == true)
        }
        x()
    }

    {
        var x: func(val: bool = true)
        x = func(val)
        {
            @assert(val == true)
        }

        x()
        x(true)
    }
}
