#test
{
    // alias is used to make a shortcut to another name or type
    {
        enum RGB { R; G; B; }
        @assert(RGB.R == 0)

        alias Color = RGB
        @assert(Color.G == 1)
    }

    // types alias
    {
        alias Float32 = f32
        alias Float64 = f64
        var x: Float32 = 1.0
        var y: Float64 = 1.0
    }

    // you can alias a function name
    {
        func thisIsABigFunctionName(x: s32) => x * x
        alias myFunc = thisIsABigFunctionName
        @assert(myFunc(4) == 16)
    }
}