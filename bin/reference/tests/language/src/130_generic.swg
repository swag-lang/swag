/**
A function can be generic by specifying parameters after `func`.
At the call site, you specify the generic parameters with `funcCall'(type1, type2, ...)(parameters)`.
Note that parenthesis can be ommited if there's only one generic parameter.
*/
#test
{
    {
        // Here 'T' is a generic type.
        func(var T) myFunc(val: T) => 2 * val

        @assert(myFunc's32(2) == 4)
        @assert(myFunc'f32(2.0) == 4.0)
    }

    {
        // You can omit 'var' to declare the generic type, because a single identifier is considered to be a type.
        func(T) myFunc(val: T) => 2 * val

        @assert(myFunc's32(2) == 4)
        @assert(myFunc'f32(2.0) == 4.0)
    }

    {
        // You can set a default value to the type.
        func(T = s32) myFunc(val: T) => 2 * val

        @assert(myFunc(2's32) == 4)
        @assert(myFunc'f32(2.0) == 4.0)
    }

    {
        // Of course you can specify more than one generic parameter
        func(K, V) myFunc(key: K, value: V) => value

        @assert(myFunc(2's32, "value") == "value")              // K and V are deduced
        @assert(myFunc'(s32, string)(2, "value") == "value")    // K and V are explicit

        @assert(myFunc(2's32, true) == true)
        @assert(myFunc'(s32, bool)(2, true) == true)
    }
}

/**
Generic types can be deduced from parameters, so `func'type()` is not always necessary.
*/
#test
{
    func(T) myFunc(val: T) => 2 * val

    @assert(myFunc(2's32) == 4)         // T is deduced to be s32
    @assert(myFunc(2.0'f32) == 4.0)     // T is deduced to be f32
}

/**
You can also specify constants as generic parameters.
*/
#test
{
    // `N` is a constant a type `s32`.
    {
        func(const N: s32) myFunc() = @assert(N == 10)
        myFunc'10()
    }

    // `const` can also be omitted, as an identifier followed by a type definition is considered to be a constant and not a type.
    {
        func(N: s32) myFunc() = @assert(N == 10)
        myFunc'10()
    }

    // You can also assign a default value to the constant.
    {
        func(N: s32 = 10) myFunc() = @assert(N == 10)
        myFunc()
    }

    // You can ommit the type if you declare the constant with `const`. It will be deduced from the assignment expression.
    {
        func(const N = 10) myFunc() = @assert(N == 10)
        myFunc()
    }
}

/**
You can mix types and constants.
*/
#test
{
    {
        // `T` is a type, `N` is a constant of type `s32`, because remember that an identifier alone is considered to be a generic type.
        func(T, N: s32) myFunc(x: T) => x * N

        alias call = myFunc'(s32, 10)
        @assert(call(2) == 20)
        @assert(call(100) == 1000)
    }

    {
        // So if you want to declare multiple constants, specify the type (or 'const') for each.
        func(T: s32, N: s32) myFunc() => T * N

        @assert(myFunc'(5, 10)() == 50)
    }

    {
        // And if you want to declare multiple types with default values, specify the value for each.
        func(T = s32, V = s32) myFunc(x: T, y: V) => x * y

        @assert(myFunc(1's32, 2'f32) == 2.0)
        @assert(myFunc(1's32, 2's32) == 2)
    }
}

/**
Like functions, a struct can also be generic.
*/
#test
{
    {
        struct(T) Struct
        {
            val: T
        }

        var x: Struct's32
        @assert(@typeof(x.val) == s32)
        var x1: Struct'f32
        @assert(@typeof(x1.val) == f32)
    }

    {
        struct(T, N: s32) Struct
        {
            val: [N] T
        }

        var x: Struct'(bool, 10)
        @assert(@typeof(x.val) == #type [10] bool)
    }
}