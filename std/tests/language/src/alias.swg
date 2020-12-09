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
        @assert(@typeof(Float32) == f32)
        @assert(@typeof(Float64) == f64)
    }

    // a type alias can be marked with the 'swag.strict' attribute
    // in that case, implicit cast is not done, but explicit cast is
    // still possible
    {
        #[swag.strict]
        alias MyType = s32
        @assert(@typeof(MyType) != s32)
        var x: MyType = cast(MyType) 0
    }

    // you can also alias a function/namespace/enum name
    {
        func thisIsABigFunctionName(x: s32) => x * x
        alias myFunc = thisIsABigFunctionName
        @assert(myFunc(4) == 16)
    }

    // alias can also be used to force the namespace name of an
    // imported module
    {
        // alias MyStd = #import "std"
    }
}