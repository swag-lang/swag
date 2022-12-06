#test
{
    {
        // A function can be generic by specifying parameters after 'func'
        // At the call site, you specify the generic parameters with funcCall'(type1, type2, ...)(parameters)
        // Note that parenthesis can be ommited if there's only one generic parameter

        // Here 'T' is a generic type
        func(T) myFunc(val: T) => 2 * val

        @assert(myFunc's32(2) == 4)
        @assert(myFunc'f32(2.0) == 4.0)
    }

    {
        // Generic types can be deduced from parameters, so func'type() is
        // not always necessary
        func(T) myFunc(val: T) => 2 * val

        @assert(myFunc(2's32) == 4)         // T is deduced to be s32
        @assert(myFunc(2.0'f32) == 4.0)     // T is deduced to be f32
    }

    {
        // Of course you can specify more than one generic parameter
        func(K, V) myFunc(key: K, value: V) => value

        @assert(myFunc(2's32, "value") == "value")              // K and V are deduced
        @assert(myFunc'(s32, string)(2, "value") == "value")    // K and V are explicit

        @assert(myFunc(2's32, true) == true)
        @assert(myFunc'(s32, bool)(2, true) == true)
    }

    // You can also specify constants as generic parameters
    {
        func(N: s32) myFunc() = @assert(N == 10)
        myFunc'10()
    }

    // And you can mix
    {
        func(T, N: s32) myFunc(x: T) => x * N

        alias call = myFunc'(s32, 10)
        @assert(call(2) == 20)
        @assert(call(100) == 1000)
    }

    // Like functions, a struct can be generic
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