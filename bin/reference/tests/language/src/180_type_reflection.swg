/**
# Types as Values in Swag

In Swag, **types are also values** that can be inspected and manipulated at both compile-time and runtime. This feature enables powerful metaprogramming capabilities. The two primary intrinsics for interacting with types are `@typeof` and `@kindof`.
*/

/**
# Using `@typeof` to Inspect Types

You can retrieve the type of an expression using `@typeof`. Additionally, you can also directly use the type itself when the expression explicitly represents a type.
*/
#test
{
    // Using @typeof to get the type of a basic type like 's8'
    let ptr1 = @typeof(s8)
    @assert(ptr1.name == "s8")
    @assert(ptr1 == s8)

    // Using @typeof to get the type of another basic type 's16'
    let ptr2 = @typeof(s16)
    @assert(ptr2.name == "s16")
    @assert(ptr2 == s16)

    // Directly using the type without @typeof
    let ptr3 = s32
    @assert(ptr3.name == "s32")
    @assert(ptr3 == @typeof(s32))

    // Another direct type usage
    let ptr4 = s64
    @assert(ptr4.name == "s64")
    @assert(ptr4 == s64)
}

/**
# Understanding the Result of `@typeof`

The result of `@typeof` is a constant pointer to a `Swag.TypeInfo` structure. This structure is a type descriptor that provides detailed information about the type. The `Swag.TypeInfo` type is a type alias for `typeinfo`, and each type has a corresponding `TypeInfo` struct found in the `Swag` namespace, part of the compiler runtime.

> NOTE:
> You can find all the type descriptors in the runtime documentation on the Swag website.
*/
#test
{
    let ptr = bool
    @assert(@typeof(ptr) == @typeof(const *Swag.TypeInfoNative))

    // '#type' can be used when the right expression is ambiguous. 
    // For example, with arrays, it might not be clear whether the expression represents a type or an array literal. 
    // In such cases, '#type' clarifies that it is a type.
    let ptr1 = #type [2] s32
    @assert(@typeof(ptr1) == @typeof(const *Swag.TypeInfoArray))
    @assert(ptr1.name == "[2] s32")

    let ptr2 = @typeof([1, 2, 3])
    @assert(@typeof(ptr2) == @typeof(const *Swag.TypeInfoArray))
    @assert(ptr2.name == "const [3] s32")
}

/**
# Working with `TypeInfo` Structures

The `TypeInfo` structure includes a `kind` field that indicates the specific kind of type (e.g., native type, pointer, array, etc.). This `kind` field is particularly useful when working with types in a more abstract way.
*/
#test
{
    let typeOf = f64
    @assert(typeOf.kind == Swag.TypeInfoKind.Native)

    // These checks can be evaluated at compile time
    using Swag
    #assert @typeof(*u8).kind == TypeInfoKind.Pointer
    #assert @typeof([1, 2, 3]).kind == TypeInfoKind.Array
    #assert @typeof({1, 2, 3}).kind == TypeInfoKind.Struct
}

/**
# `@decltype`

`@decltype` is the reverse of `@typeof` or `@kindof`. It is used to convert a `typeinfo` back into an actual compiler type.
*/
#test
{
    var x: @decltype(@typeof(s32))
    #assert @typeof(x) == s32
}

/**
# Using `@decltype` with Compile-Time Expressions

`@decltype` can evaluate a *constexpr* expression that returns a `typeinfo` to determine the actual type. This is useful when the type depends on compile-time logic.
*/
#test
{
    #[Swag.ConstExpr]
    func getType(needAString: bool) -> typeinfo
    {
        if needAString:
            return string
        else:
            return s32
    }

    var x: @decltype(getType(needAString: false))
    #assert @typeof(x) == s32
    x = 0

    var x1: @decltype(getType(needAString: true))
    #assert @typeof(x1) == string
    x1 = "0"
}
