#test
{
    {
        // A function can be generic by specifying parameters after 'func'
        // At the call site, you specify the generic parameters with funcCall'(type1, type2, ...)(parameters)
        // Note that parenthesis can be ommited if there's only one generic parameter
        func(T) myFunc(val: T) => 2 * val

        @assert(myFunc's32(2) == 4)
        @assert(myFunc'f32(2.0) == 4.0)
    }

    {
        // Generic types can be deduced from parameters, so func'type() is
        // not always necessary
        func(T) myFunc(val: T) => 2 * val

        @assert(myFunc(2's32) == 4)
        @assert(myFunc(2.0'f32) == 4.0)
    }

    {
        // You can specify more than one generic parameter
        func(K, V) myFunc(key: K, value: V) => value

        @assert(myFunc(2's32, "value") == "value")
        @assert(myFunc'(s32, string)(2, "value") == "value")

        @assert(myFunc(2's32, true) == true)
        @assert(myFunc'(s32, bool)(2, true) == true)
    }

    {
        // You can also specify constants as generic parameters
        func(N: s32) myFunc() = @assert(N == 10)

        myFunc'10()
    }

    {
        // And you can mix
        func(T, N: s32) myFunc(x: T) => x * N

        alias call = myFunc'(s32, 10)
        @assert(call(2) == 20)
        @assert(call(100) == 1000)
    }
}