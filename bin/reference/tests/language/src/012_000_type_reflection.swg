/**
# Types as Values in Swag

In Swag, **types are treated as first-class values** that can be inspected and manipulated
at both compile-time and runtime. This powerful feature enables advanced metaprogramming
capabilities, allowing developers to write more flexible and reusable code. The primary
intrinsics for interacting with types are `#typeof` and `@kindof`, which provide the ability
to introspect and work with types dynamically.
*/

/**
# Using `#typeof` to Inspect Types

The `#typeof` intrinsic allows you to retrieve the type information of an expression.
When an expression explicitly represents a type, you can also directly use the type
itself. This is particularly useful for type inspection and validation at compile time.
*/
#test
{
    // Using #typeof to retrieve the type information of the basic type 's8'
    let ptr1 = #typeof(s8)
    @assert(ptr1.name == "s8") // Verifies that the name of the type is 's8'
    @assert(ptr1 == s8)        // Confirms that the retrieved type matches 's8'

    // Retrieving the type of another basic type 's16' using #typeof
    let ptr2 = #typeof(s16)
    @assert(ptr2.name == "s16")
    @assert(ptr2 == s16)

    // Directly using the type 's32' without #typeof
    let ptr3 = s32
    @assert(ptr3.name == "s32")
    @assert(ptr3 == #typeof(s32))

    // Another example of direct type usage with 's64'
    let ptr4 = s64
    @assert(ptr4.name == "s64")
    @assert(ptr4 == s64)
}

/**
# Understanding the Result of `#typeof`

The result of the `#typeof` intrinsic is a constant pointer to a `Swag.TypeInfo` structure.
This structure serves as a type descriptor, providing detailed information about the type
it describes. The `Swag.TypeInfo` is an alias for the `typeinfo` type, and each type in Swag
corresponds to a specific `TypeInfo` structure found in the `Swag` namespace, which is
part of the compiler runtime.

> NOTE:
> You can explore all available type descriptors in the runtime documentation on the Swag website.
*/
#test
{
    // Using #typeof on a bool type, which is a native type
    let ptr = bool
    @assert(#typeof(ptr) == #typeof(const *Swag.TypeInfoNative))

    // The `#type` keyword is used when the expression might be ambiguous, such as with arrays.
    // This ensures that the compiler understands the expression as a type.
    let ptr1 = #type [2] s32
    @assert(#typeof(ptr1) == #typeof(const *Swag.TypeInfoArray))
    @assert(ptr1.name == "[2] s32")

    // Using #typeof on an array literal
    let ptr2 = #typeof([1, 2, 3])
    @assert(#typeof(ptr2) == #typeof(const *Swag.TypeInfoArray))
    @assert(ptr2.name == "const [3] s32")
}

/**
# Working with `TypeInfo` Structures

The `TypeInfo` structure includes a `kind` field that identifies the specific category
of the type, such as native type, pointer, array, or struct. This `kind` field is crucial
when dealing with types in a more abstract or generic manner, as it allows you to differentiate
between various kinds of types and handle them appropriately.
*/
#test
{
    // Checking the 'kind' field of the typeinfo for 'f64', which should be a native type
    let typeOf = f64
    @assert(typeOf.kind == Swag.TypeInfoKind.Native)

    // Evaluating these type checks at compile time
    using Swag
    #assert(#typeof(*u8).kind == TypeInfoKind.Pointer)  // Ensures the type is recognized as a pointer
    #assert(#typeof([1, 2, 3]).kind == TypeInfoKind.Array)  // Ensures the type is recognized as an array
    #assert(#typeof({1, 2, 3}).kind == TypeInfoKind.Struct)  // Ensures the type is recognized as a struct
}

/**
# `#decltype`

The `#decltype` intrinsic performs the reverse operation of `#typeof` or `@kindof`.
It converts a `typeinfo` structure back into an actual compiler type. This is useful
when you need to dynamically determine the type of a variable based on compile-time
information and then use that type within your code.
*/
#test
{
    // Using #decltype to declare a variable of type s32 based on its typeinfo
    var x: #decltype(#typeof(s32))
    #assert(#typeof(x) == s32)  // Confirms that the variable 'x' is indeed of type 's32'
}

/**
# Using `#decltype` with Compile-Time Expressions

`#decltype` can evaluate a compile-time constant expression (*constexpr*) that returns
a `typeinfo` to determine the actual type. This is particularly powerful when the type
depends on compile-time logic, allowing for dynamic yet type-safe programming patterns.
*/
#test
{
    // A function that returns a typeinfo based on a compile-time condition
    #[Swag.ConstExpr]
    func getType(needAString: bool) -> typeinfo
    {
        if needAString:
            return string
        else:
            return s32
    }

    // Using #decltype to determine the type of 'x' based on the compile-time condition
    var x: #decltype(getType(needAString: false))
    #assert(#typeof(x) == s32)  // Confirms that 'x' is of type s32
    x = 0

    // Another example with the condition evaluating to true
    var x1: #decltype(getType(needAString: true))
    #assert(#typeof(x1) == string)  // Confirms that 'x1' is of type string
    x1 = "0"
}
