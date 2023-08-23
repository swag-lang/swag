/**
In Swag, types are also values that can be inspected at compile time or at runtime. The two main intrinsics for this are `@typeof` and `@kindof`.
*/

/**
You can get the type of an expression with `@typeof`, or just with the type itself (**types are also values**).
*/
#test
{
    let ptr1 = @typeof(s8)
    @assert(ptr1.name == "s8")
    @assert(ptr1 == s8)

    let ptr2 = @typeof(s16)
    @assert(ptr2.name == "s16")
    @assert(ptr2 == s16)

    let ptr3 = s32
    @assert(ptr3.name == "s32")
    @assert(ptr3 == @typeof(s32))

    let ptr4 = s64
    @assert(ptr4.name == "s64")
    @assert(ptr4 == s64)
}

/**
The return result of `@typeof` is a const pointer to a `Swag.TypeInfo` kind of structure. This is an typealias for the `typeinfo` type.
*/
#test
{
    let ptr = bool
    @assert(@typeof(ptr) == @typeof(const *Swag.TypeInfoNative))

    let ptr1 = #type [2] s32
    @assert(@typeof(ptr1) == @typeof(const *Swag.TypeInfoArray))
    @assert(ptr1.name == "[2] s32")
}

/**
The `TypeInfo` structure contains a different enum value for each type.
*/
#test
{
    let ptr = f64
    @assert(@typeof(ptr.kind).fullname == "Swag.TypeInfoKind")
    @assert(ptr.sizeof == @sizeof(f64))
}


/**
# @decltype
*/

/**
`@decltype` can be used to transform a `typeinfo` to a real compiler type. This is the opposite of `@typeof` or `@kindof`.
*/
#test
{
    var x: @decltype(@typeof(s32))
    #assert @typeof(x) == s32
}

/**
`@decltype` can evaluate a constexpr expression that returns a `typeinfo` to determine the real type.
*/
#test
{
    #[Swag.ConstExpr]
    func getType(needAString: bool)->typeinfo
    {
        if needAString
            return string
        else
            return s32
    }

    var x: @decltype(getType(needAString: false))
    #assert @typeof(x) == s32
    x = 0

    var x1: @decltype(getType(needAString: true))
    #assert @typeof(x1) == string
    x1 = "0"
}
