#test
{
    // A lambda is just a pointer to a function
    {
        func myFunction0() {}
        func myFunction1(x: s32) => x * x

        // 'ptr0' is a pointer to a function that takes no parameter, and returns nothing
        var ptr0: func() = &myFunction0
        ptr0()

        // Here type of 'ptr1' is deduced from 'myFunction1'
        ptr1 := &myFunction1
        @assert(myFunction1(2) == 4)
        @assert(ptr1(2) == 4)
    }

    // A lambda can be null
    {
        var lambda: func()->bool
        @assert(lambda == null)
    }

    // You can use lambda as a function parameter type
    {
        alias callback = func(s32)->s32
        func toDo(value: s32, ptr: callback)->s32 => ptr(value)

        func square(x: s32) => x * x
        @assert(toDo(4, &square) == 16)
    }

    // You can also create anonymous functions with '@' (functions as literals)
    {
        cb := @(x: s32)->s32 => x * x
        @assert(cb(4) == 16)
        cb = @(x: s32)->s32 => x * x * x
        @assert(cb(4) == 64)
    }

    // Anonymous functions can be passed as parameters to another function
    {
        alias callback = func(s32)->s32
        func toDo(value: s32, ptr: callback)->s32 => ptr(value)

        @assert(toDo(4, @(x: s32) => x * x) == 16)
        @assert(toDo(4, @(x: s32) => x + x) == 8)
        @assert(toDo(4, @(x: s32)->s32 { return x - x; }) == 0)
    }

    // Types of parameters can be deduced
    {
        alias callback = func(s32)->s32
        func toDo(value: s32, ptr: callback)->s32 => ptr(value)

        @assert(toDo(4, @(x) => x * x) == 16)
        @assert(toDo(4, @(x) => x + x) == 8)
        @assert(toDo(4, @(x)->s32 { return x - x; }) == 0)
    }
}