/**
A lambda is just a **pointer to a function**.
*/
#test
{
    func myFunction0() {}
    func myFunction1(x: s32) => x * x

    // 'ptr0' is declared as a pointer to a function that takes no parameter, and returns nothing
    var ptr0: func() = &myFunction0
    ptr0()

    // Here type of 'ptr1' is deduced from 'myFunction1'
    var ptr1 = &myFunction1
    @assert(myFunction1(2) == 4)
    @assert(ptr1(2) == 4)
}

/**
A lambda can be null.
*/
#test
{
    var lambda: func()->bool
    @assert(lambda == null)
}

/**
You can use a lambda as a function parameter type.
*/
#test
{
    alias callback = func(s32)->s32
    func toDo(value: s32, ptr: callback)->s32 => ptr(value)

    func square(x: s32) => x * x
    @assert(toDo(4, &square) == 16)
}

/**
# Anonymous functions
You can also create *anonymous functions* (aka functions as literals).
*/
#test
{
    var cb = func(x: s32)->s32 => x * x
    @assert(cb(4) == 16)
    cb = func(x: s32)->s32 => x * x * x
    @assert(cb(4) == 64)
}

/**
Anonymous functions can be passed as parameters to another function.
*/
#test
{
    alias callback = func(s32)->s32
    func toDo(value: s32, ptr: callback)->s32 => ptr(value)

    @assert(toDo(4, func(x: s32) => x * x) == 16)
    @assert(toDo(4, func(x: s32) => x + x) == 8)
    @assert(toDo(4, func(x: s32)->s32 { return x - x; }) == 0)
}

/**
The types of the parameters can be deduced.
*/
#test
{
    alias callback = func(s32)->s32
    func toDo(value: s32, ptr: callback)->s32 => ptr(value)

    @assert(toDo(4, func(x) => x * x) == 16)
    @assert(toDo(4, func(x) => x + x) == 8)
    @assert(toDo(4, func(x) { return x - x; }) == 0)
}

/**
When you affect a lambda to a variable, the type of parameters and the return type can also be omitted, as they will be deduced from the variable type.
*/
#test
{
    var fct: func(s32, s32)->bool

    fct = func(x, y) => x == y
    @assert(fct(10, 10))

    fct = func(x, y) { return x != y; }
    @assert(fct(20, 120))
}

/**
Lambdas can have default parameters values.
*/
#test
{
    {
        var x = func(val = true) {
            @assert(val == true)
        }

        x()
    }

    {
        var x: func(val: bool = true)

        x = func(val) {
            @assert(val == true)
        }

        x()
        x(true)
    }
}