#test
{
    // `typealias` is used to make a shortcut to another type.
    {
        enum RGB { R; G; B; }
        @assert(RGB.R == 0)

        typealias Color = RGB
        @assert(Color.G == 1)
    }

    {
        typealias Float32 = f32
        typealias Float64 = f64
        var x: Float32 = 1.0
        var y: Float64 = 1.0
        #assert @typeof(Float32) == f32
        #assert @typeof(Float64) == f64
    }

    // A type typealias can be marked with the `Swag.Strict` attribute. In that case, implicit cast is not done, but explicit cast is still possible.
    {
        #[Swag.Strict]
        typealias MyType = s32
        #assert @typeof(MyType) != s32
        var x: MyType = cast(MyType) 0
    }
}

#test
{
    // You can also alias a function with 'namealias"
    {
        func thisIsABigFunctionName(x: s32) => x * x
        namealias myFunc = thisIsABigFunctionName
        @assert(myFunc(4) == 16)
    }

    // You can also alias a variable.
    {
        var myLongVariableName: s32 = 0
        namealias short = myLongVariableName
        short += 2
        @assert(myLongVariableName == 2)
    }
}