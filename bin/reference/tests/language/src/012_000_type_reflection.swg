/**
# 'Types as Values in Swag'

In Swag, types are treated as first-class values that can be inspected and
manipulated at both compile time and runtime. This enables powerful
metaprogramming patterns for flexible, reusable code.

The primary intrinsics for interacting with types are '#typeof' and '@kindof',
which let you introspect and work with types dynamically.
*/

/**
# Using '#typeof' to Inspect Types

The '#typeof' intrinsic retrieves the type information of an expression.
When an expression explicitly represents a type, you can also use the type
itself. This is useful for compile-time inspection and validation.
*/
#test
{
    // Basic types via '#typeof' and direct type usage
    let ptr1 = #typeof(s8)
    @assert(ptr1.name == "s8")
    @assert(ptr1 == s8)

    let ptr2 = #typeof(s16)
    @assert(ptr2.name == "s16")
    @assert(ptr2 == s16)

    let ptr3 = s32
    @assert(ptr3.name == "s32")
    @assert(ptr3 == #typeof(s32))

    let ptr4 = s64
    @assert(ptr4.name == "s64")
    @assert(ptr4 == s64)
}

/**
# Understanding the Result of '#typeof'

'#typeof' yields a constant pointer to a 'Swag.TypeInfo' structure (alias of
'typeinfo'). Each Swag type maps to a specific 'TypeInfo' descriptor in the
'Swag' namespace, which is part of the compiler runtime.

> NOTE:
> You can explore all type descriptors in the runtime documentation on the Swag website.
*/
#test
{
    // Native type -> native typeinfo
    let ptr = bool
    @assert(#typeof(ptr) == #typeof(const *Swag.TypeInfoNative))

    // Use '#type' to disambiguate when the expression could be parsed as a value
    let ptr1 = #type [2] s32
    @assert(#typeof(ptr1) == #typeof(const *Swag.TypeInfoArray))
    @assert(ptr1.name == "[2] s32")

    // Array literal -> array typeinfo
    let ptr2 = #typeof([1, 2, 3])
    @assert(#typeof(ptr2) == #typeof(const *Swag.TypeInfoArray))
    @assert(ptr2.name == "const [3] s32")
}

/**
# Working with 'TypeInfo' Structures

'TypeInfo' exposes a 'kind' field identifying the category: native, pointer,
array, struct, etc. This is essential when handling types generically.
*/
#test
{
    // 'f64' is a native type
    let typeOf = f64
    @assert(typeOf.kind == Swag.TypeInfoKind.Native)

    // Compile-time checks of kind
    using Swag
    #assert(#typeof(*u8).kind == TypeInfoKind.Pointer) // Pointer
    #assert(#typeof([1, 2, 3]).kind == TypeInfoKind.Array) // Array
    #assert(#typeof({1, 2, 3}).kind == TypeInfoKind.Struct) // Struct
}

/**
# '#decltype'

'#decltype' performs the reverse of '#typeof'/'@kindof': it converts a
'typeinfo' back into a compiler type. Use it to materialize a type determined
by compile-time information.
*/
#test
{
    // Create a variable whose type is resolved from typeinfo
    var x: #decltype(#typeof(s32))
    #assert(#typeof(x) == s32)
}

/**
# Using '#decltype' with Compile-Time Expressions

'#decltype' can evaluate a constexpr that returns a 'typeinfo' and materialize
the corresponding type. This enables dynamic yet type-safe patterns.
*/
#test
{
    // Return a typeinfo based on a constexpr condition
    #[Swag.ConstExpr]
    func getType(needAString: bool)->typeinfo
    {
        if needAString do
            return string
        else do
            return s32
    }

    // Materialize the chosen type via '#decltype'
    var x: #decltype(getType(needAString: false))
    #assert(#typeof(x) == s32)
    x = 0

    var x1: #decltype(getType(needAString: true))
    #assert(#typeof(x1) == string)
    x1 = "0"
}
