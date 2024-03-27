/**
# Type alias
*/

/**
`typealias` is used to make a shortcut to another type.
*/
#test
{
    enum RGB { R; G; B; }
    @assert(RGB.R == 0)

    typealias Color = RGB   // 'Color' is now equivalent to 'RGB'
    @assert(Color.G == 1)
}

/**
You can then use the new name in place of the original type. This does not create a new type.
*/
#test
{
    typealias Float32 = f32
    typealias Float64 = f64

    var x: Float32 = 1.0    // Same as 'f32'
    var y: Float64 = 1.0    // Same as 'f64'
    #assert @typeof(Float32) == f32
    #assert @typeof(Float64) == f64
}

/**
But to create a new type, a `typealias` can also be marked with the `Swag.Strict` attribute. In that case, all implicit casts won't be done. Explicit cast are still possible.
*/
#test
{
    #[Swag.Strict]
    typealias MyType = s32
    #assert @typeof(MyType) != s32
    var x: MyType = cast(MyType) 0
}


/**
# Name alias
*/

/**
You can alias a function name with `namealias`
*/
#test
{
    func thisIsABigFunctionName(x: s32) => x * x
    namealias myFunc = thisIsABigFunctionName
    @assert(myFunc(4) == 16)
}

/**
You can also alias variables and namespaces
*/
#test
{
    var myLongVariableName: s32 = 0
    namealias short = myLongVariableName
    short += 2
    @assert(myLongVariableName == 2)
}
