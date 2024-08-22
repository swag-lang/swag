/**
A lambda in Swag is essentially a **pointer to a function**.
*/
#test
{
    func myFunction0() {}
    func myFunction1(x: s32) => x * x

    // 'ptr0' is a pointer to a function that takes no parameters and returns nothing.
    let ptr0: func() = &myFunction0
    ptr0()

    // The type of 'ptr1' is inferred from 'myFunction1'.
    let ptr1 = &myFunction1
    @assert(myFunction1(2) == 4)
    @assert(ptr1(2) == 4)
}

/**
A lambda can also be null.
*/
#test
{
    var lambda: func() -> bool
    @assert(lambda == null)
}

/**
Lambdas can be used as function parameter types.
*/
#test
{
    typealias Callback = func(s32) -> s32
    func toDo(value: s32, ptr: Callback) -> s32 => ptr(value)

    func square(x: s32) => x * x
    @assert(toDo(4, &square) == 16)
}

/**
# Anonymous Functions
You can also create *anonymous functions* (also known as function literals).
*/
#test
{
    var cb = func(x: s32) -> s32 => x * x
    @assert(cb(4) == 16)
    cb = func(x: s32) -> s32 => x * x * x
    @assert(cb(4) == 64)
}

/**
Anonymous functions can be passed directly as parameters to other functions.
*/
#test
{
    typealias Callback = func(s32) -> s32
    func toDo(value: s32, ptr: Callback) -> s32 => ptr(value)

    @assert(toDo(4, func(x: s32) => x * x) == 16)
    @assert(toDo(4, func(x: s32) => x + x) == 8)
    @assert(toDo(4, func(x: s32) -> s32 { return x - x; }) == 0)
}

/**
Parameter types in anonymous functions can be inferred.
*/
#test
{
    typealias Callback = func(s32) -> s32
    func toDo(value: s32, ptr: Callback) -> s32 => ptr(value)

    @assert(toDo(4, func(x) => x * x) == 16)
    @assert(toDo(4, func(x) => x + x) == 8)
    @assert(toDo(4, func(x) { return x - x; }) == 0)
}

/**
When assigning a lambda to a variable, the types of parameters and the return type can also be omitted, as they will be inferred from the variable's type.
*/
#test
{
    var fct: func(s32, s32) -> bool

    fct = func(x, y) => x == y
    @assert(fct(10, 10))

    fct = func(x, y) { return x != y; }
    @assert(fct(20, 120))
}

/**
Lambdas can have default parameter values.
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
