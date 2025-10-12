/**
# Enums in Swag

Enums define a set of named constant values. Unlike C/C++, Swag allows enum values to end with `;`, `,`, or simply a new line.
*/
#test
{
    enum Values0 { A, B }
    enum Values1 { A, B }
    enum Values2 { A, B }
    enum Values3 { A, B }
}

/**
# Enum Underlying Type

By default, enums use the `s32` type for storage.
*/
#test
{
    enum Values { A, B }
    let type = #typeof(Values)

    @assert(type.rawType == s32)
    #assert(#typeof(Values) == Values)
}

/**
# Custom Enum Underlying Type

A custom base type can be specified after the enum name.
*/
#test
{
    enum Values1: s64
    {
        A
        B
        C
    }

    #assert(#typeof(Values1).rawType == s64)
    #assert(#typeof(Values1.A) == Values1)
}

/**
# Default and Custom Enum Values

Without explicit values, enums start at 0 and increment by 1.
*/
#test
{
    enum Value: s64 { A, B, C }
    #assert(Value.A == 0)
    #assert(Value.B == 1)
    #assert(Value.C == 2)
}

/**
Custom values can be assigned manually.
*/
#test
{
    enum Value: s64 { A = 10, B = 20, C = 30 }
    #assert(Value.A == 10)
    #assert(Value.B == 20)
    #assert(Value.C == 30)
}

/**
# Incremental Enum Values

After a custom value, following values auto-increment.
*/
#test
{
    enum Value: u32
    {
        A = 10
        B
        C
    }

    #assert(Value.A == 10)
    #assert(Value.B == 11)
    #assert(Value.C == 12)
}

/**
# Non-Integer Enum Values

Non-integer enums require explicit assignments.
*/
#test
{
    enum Value1: string { A = "string 1", B = "string 2", C = "string 3" }
    #assert(Value1.A == "string 1")

    enum Value2: f32
    {
        A = 1.0
        B = 3.14
        C = 6.0
    }

    #assert(Value2.B == 3.14)
}

/**
# Counting Enum Values

`@countof` returns the number of enum members.
*/
#test
{
    enum Value: string
    {
        A = "1"
        B = "2"
        C = "3"
    }

    @assert(@countof(Value) == 3)
}

/**
# Using `using` with Enums

`using` allows direct access to enum values without prefixing with the enum name.
*/
#test
{
    enum Value { A, B, C }
    using Value

    @assert(A == 0)
    @assert(B == 1)
    @assert(Value.C == 2)
}

/**
# Enums as Flags

With `#[Swag.EnumFlags]`, enum values represent bit flags. They should use unsigned integer types.
*/
#test
{
    #[Swag.EnumFlags]
    enum MyFlags: u8 { A, B, C, D }

    #assert(MyFlags.A == 0b00000001)
    #assert(MyFlags.B == 0b00000010)
    #assert(MyFlags.C == 0b00000100)

    let value = MyFlags.B | MyFlags.C
    @assert(value == 0b00000110)
}

/**
# Enums with Arrays

Enums can store array constants.
*/
#test
{
    enum Value: const [2] s32 { A = [1, 2], B = [10, 20] }
    #assert(Value.A[0] == 1)
    #assert(Value.B[1] == 20)
}

/**
# Enums with Slices

Enums can also store slices.
*/
#test
{
    enum Value: const [..] s32 { A = [1, 2], B = [10, 20, 30] }
    #assert(@countof(Value.A) == 2)
    @assert(Value.B[2] == 30)
}

/**
# Nested Enums

Enums can include other enums using `using`. Both must share the same base type.
*/
enum BasicErrors { FailedToLoad, FailedToSave }

enum MyErrors
{
    using BasicErrors
    NotFound = 100
}

/**
# Accessing Nested Enums

Access nested values with their scoped names.
*/
const MyError0 = MyErrors.BasicErrors.FailedToSave

/**
# Automatic Cast with Nested Enums

Nested enums can be automatically cast to their parent enum type.
*/
#test
{
    const E0: MyErrors = MyErrors.BasicErrors.FailedToLoad
    const E1: BasicErrors = BasicErrors.FailedToLoad

    func toto(err: MyErrors)
    {
        @assert(err == BasicErrors.FailedToLoad)
    }

    toto(E0)
    toto(E1)
}

/**
# Specific Enum Attributes

`#[Swag.EnumIndex]` allows direct indexing with enum values.
*/
#test
{
    #[Swag.EnumIndex]
    enum MyIndex { First, Second, Third }

    const Array = [0, 1, 2]
    const Valu  = Array[MyIndex.First]
}

/**
`#[Swag.NoDuplicate]` prevents duplicate values.
*/
#test
{
    #[Swag.NoDuplicate]
    enum MyEnum { Val0 = 0 }
}

/**
# Enum Type Inference

Swag infers enum types automatically in assignments and expressions.
*/
#test
{
    enum Values { A, B }
    let x: Values = Values.A
    let y: Values = A
    @assert(x == y)
}

/**
# Type Inference in `switch`

Enum types are inferred inside `switch` statements.
*/
#test
{
    enum Values { A, B }
    let x = Values.A

    switch x
    {
    case A:
        break
    case B:
        break
    }
}

/**
# Simplified Enum Syntax

You can omit the enum name when the type is already known.
*/
#test
{
    enum Values { A, B }
    let x = Values.A
    @assert(x == .A)
}

/**
# Simplified Enum Flags Syntax

This works with flags too.
*/
#test
{
    #[Swag.EnumFlags]
    enum Values { A, B }

    let x = Values.A | Values.B
    @assert((x & .A) and (x & .B))
}

/**
# Simplified Enum Syntax in Functions
*/
#test
{
    enum Values { A, B }
    func toto(v1, v2: Values) {}
    toto(.A, .B)
}

/**
# Iterating Over Enum Values

You can iterate over enums using `for` or `foreach`.
*/
#test
{
    enum RGB { R, G, B }

    var cpt = 0
    for idx in RGB do
        cpt += 1
    @assert(cpt == 3)
}

/**
`foreach` offers a structured way to iterate and handle specific values.
*/
#test
{
    enum RGB { R, G, B }

    foreach val in RGB
    {
        switch val
        {
        case R:
            break
        case G:
            break
        case B:
            break
        default:
            @assert(false)
        }
    }
}
