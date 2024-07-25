/**
In Swag, **types are also values** that can be inspected at compile time or at runtime. The two main intrinsics for this are `@typeof` and `@kindof`.
*/

/**
You can get the type of an expression with `@typeof`, or just with the type itself.
*/
#test
{
    let ptr1 = @typeof(s8)
    @assert(ptr1.name == "s8")
    @assert(ptr1 == s8)

    let ptr2 = @typeof(s16)
    @assert(ptr2.name == "s16")
    @assert(ptr2 == s16)

    // See that '@typeof' is not really necessary if the expression on the right is a type.
    let ptr3 = s32
    @assert(ptr3.name == "s32")
    @assert(ptr3 == @typeof(s32))

    let ptr4 = s64
    @assert(ptr4.name == "s64")
    @assert(ptr4 == s64)
}

/**
The return result of `@typeof` is a const pointer to a `Swag.TypeInfo` kind of structure. This is a typealias for the `typeinfo` type. All types have a corresponding struct that can be found in the 'Swag' namespace, which is part of the compiler runtime.

> NOTE:
> You can find all the type descriptors in the runtime [documentation](https://www.swag-lang.org/swag.runtime.php)
*/
#test
{
    let ptr = bool
    @assert(@typeof(ptr) == @typeof(const *Swag.TypeInfoNative))

    // '#type' can be used when the right expression is ambiguous. In the case of arrays, it could be
    // a type or the start of an array literal, so '#type' tells the compiler this is a type.
    let ptr1 = #type [2] s32
    @assert(@typeof(ptr1) == @typeof(const *Swag.TypeInfoArray))
    @assert(ptr1.name == "[2] s32")

    let ptr2 = @typeof([1, 2, 3])
    @assert(@typeof(ptr2) == @typeof(const *Swag.TypeInfoArray))
    @assert(ptr2.name == "const [3] s32")
}

/**
The `TypeInfo` structure contains a different enum value for each type.
*/
#test
{
    let typeOf = f64
    @assert(typeOf.kind == Swag.TypeInfoKind.Native)

    // This can be evaluated compile-time
    using Swag
    #assert @typeof(*u8).kind == TypeInfoKind.Pointer
    #assert @typeof([1, 2, 3]).kind == TypeInfoKind.Array
    #assert @typeof({1, 2, 3}).kind == TypeInfoKind.Struct
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
`@decltype` can evaluate a *constexpr* expression that returns a `typeinfo` to determine the real type.
*/
#test
{
    #[Swag.ConstExpr]
    func getType(needAString: bool)->typeinfo
    {
        if needAString do
            return string
        else do
            return s32
    }

    var x: @decltype(getType(needAString: false))
    #assert @typeof(x) == s32
    x = 0

    var x1: @decltype(getType(needAString: true))
    #assert @typeof(x1) == string
    x1 = "0"
}
