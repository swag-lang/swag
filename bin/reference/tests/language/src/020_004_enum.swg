/**
# Enums in Swag

Enums in Swag allow you to define a set of named values. Unlike C/C++, enum values in Swag can end with `;`, `,`, or an end of line (eol).
*/
#test
{
    enum Values0
    {
        A
        B
    }

    enum Values1
    {
        A,
        B,
    }

    // The last comma is not necessary
    enum Values2
    {
        A,
        B
    }

    enum Values3 { A, B }
    enum Values4 { A; B; }
    enum Values5 { A; B }
}

/**
# Enum Underlying Type

By default, an enum in Swag is of type `s32`, meaning the underlying storage for each value is a 32-bit signed integer.
*/
#test
{
    enum Values { A, B }
    let type = @typeof(Values)

    // 'type' here is a 'typeinfo' dedicated to the enum type.
    // In that case, 'type.rawType' represents the enum's underlying type.
    @assert(type.rawType == s32)
    #assert @typeof(Values) == Values
}

/**
# Using `@kindof` to Retrieve Underlying Type

`@kindof` can be used to return the underlying type of the enum at runtime.
*/
#test
{
    enum RGB { R, G, B }
    #assert @typeof(RGB) == RGB
    #assert @kindof(RGB) != RGB
    #assert @kindof(RGB) == s32
}

/**
# Custom Enum Underlying Type

You can specify a custom underlying type for an enum by appending it after the enum name.
*/
#test
{
    enum Values1: s64 // Force the underlying type to be 's64' instead of 's32'
    {
        A
        B
        C
    }

    #assert @kindof(Values1) == s64
    #assert @typeof(Values1.A) == Values1
}

/**
# Default and Custom Enum Values

Enum values, if not explicitly specified, start at 0 and increase by 1 for each subsequent value.
*/
#test
{
    enum Value: s64
    {
        A
        B
        C
    }

    #assert Value.A == 0
    #assert Value.B == 1
    #assert Value.C == 2
}

/**
You can specify custom values for each enum element.
*/
#test
{
    enum Value: s64
    {
        A = 10
        B = 20
        C = 30
    }

    #assert Value.A == 10
    #assert Value.B == 20
    #assert Value.C == 30
}

/**
# Incremental Enum Values

If you omit a value after assigning a specific one, it will be assigned the previous value plus 1.
*/
#test
{
    enum Value: u32
    {
        A = 10
        B // Will be 11
        C // Will be 12
    }

    #assert Value.A == 10
    #assert Value.B == 11
    #assert Value.C == 12
}

/**
# Non-Integer Enum Values

For non-integer types, you **must** specify the values explicitly, as they cannot be deduced automatically.
*/
#test
{
    enum Value1: string
    {
        A = "string 1"
        B = "string 2"
        C = "string 3"
    }

    #assert Value1.A == "string 1"
    #assert Value1.B == "string 2"
    #assert Value1.C == "string 3"

    enum Value2: f32
    {
        A = 1.0
        B = 3.14
        C = 6.0
    }

    #assert Value2.A == 1.0
    #assert Value2.B == 3.14
    #assert Value2.C == 6.0
}

/**
# Counting Enum Values

`@countof` can be used to return the number of values defined in an enum.
*/
#test
{
    enum Value: string
    {
        A = "string 1"
        B = "string 2"
        C = "string 3"
    }

    @assert(@countof(Value) == 3)
    #assert @countof(Value) == 3
}

/**
# Using `using` with Enums

You can use the `using` keyword to make enum values accessible without needing to specify the enum name.
*/
#test
{
    enum Value
    {
        A
        B
        C
    }

    using Value

    // No need to prefix with 'Value.'
    @assert(A == 0)
    @assert(B == 1)
    @assert(C == 2)

    // The normal form is still possible
    @assert(Value.A == 0)
    @assert(Value.B == 1)
    @assert(Value.C == 2)
}

/**
# Enums as Flags

Enums can be used as flags if declared with the `#[Swag.EnumFlags]` attribute. For this, the enum's underlying type should be `u8`, `u16`, `u32`, or `u64`. By default, enums declared as flags start at 1 (not 0), and each value is typically a power of 2.
*/
#test
{
    #[Swag.EnumFlags]
    enum MyFlags: u8
    {
        A   // First value is 1
        B   // Value is 2
        C   // Value is 4
        D   // Value is 8
    }

    #assert MyFlags.A == 0b00000001
    #assert MyFlags.B == 0b00000010
    #assert MyFlags.C == 0b00000100
    #assert MyFlags.D == 0b00001000

    let value = MyFlags.B | MyFlags.C
    @assert(value == 0b00000110)
    @assert(value & MyFlags.B == MyFlags.B)
    @assert(value & MyFlags.C == MyFlags.C)
}

/**
# Enums with Arrays

You can define an enum where each value is associated with a const static array.
*/
#test
{
    enum Value: const [2] s32
    {
        A = [1, 2]
        B = [10, 20]
    }

    #assert Value.A[0] == 1
    #assert Value.A[1] == 2
    #assert Value.B[0] == 10
    #assert Value.B[1] == 20
}

/**
# Enums with Slices

Similarly, you can define an enum where each value is associated with a const slice.
*/
#test
{
    enum Value: const [..] s32
    {
        A = [1, 2]
        B = [10, 20, 30, 40]
    }

    #assert @countof(Value.A) == 2
    #assert @countof(Value.B) == 4

    let x = Value.A
    @assert(x[0] == 1)
    @assert(x[1] == 2)
    let y = Value.B
    @assert(y[0] == 10)
    @assert(y[1] == 20)
}

/**
# Nested Enums

Enums can be nested within other enums using the `using` keyword. Both enums must share the same underlying type.
*/
enum BasicErrors
{
    FailedToLoad
    FailedToSave
}

// The enum 'BasicErrors' is nested inside 'MyErrors'
enum MyErrors
{
    using BasicErrors
    NotFound = 100
}

/**
# Accessing Nested Enums

To access a value inside a nested enum, use the enum name (a scope is created).
*/
const MyError0 = MyErrors.BasicErrors.FailedToSave

/**
# Automatic Cast with Nested Enums

An **automatic cast** occurs when converting a nested enum to its parent enum. For example, a value of type `BasicErrors` can be passed to a parameter of type `MyErrors` because `MyErrors` includes `BasicErrors`.
*/
#test
{
    const E0: MyErrors    = MyErrors.BasicErrors.FailedToLoad
    const E1: BasicErrors = BasicErrors.FailedToLoad

    func toto(err: MyErrors)
    {
        @assert(err == BasicErrors.FailedToLoad)
        @assert(err == MyErrors.BasicErrors.FailedToLoad)
    }

    toto(E0)
    toto(E1) // Automatic cast from 'BasicErrors' to 'MyErrors'
}

/**
# Specific Attributes for Enums

You can use `#[Swag.EnumIndex]` to enable an enum value to be used directly as an index without needing to cast it. The underlying enum type must be an integer type.
*/
#test
{
    #[Swag.EnumIndex]
    enum MyIndex { First, Second, Third }

    const Array = [0, 1, 2]
    const Valu = Array[MyIndex.First]   // No need to cast 'MyIndex.First'
}

/**
# Preventing Duplicate Enum Values

You can use `#[Swag.NoDuplicate]` to prevent duplicated values inside an enum. If the compiler detects a value defined more than once, it will raise an error.
*/
#test
{
    #[Swag.NoDuplicate]
    enum MyEnum
    {
        Val0 = 0
        //Val1 = 0      // Will raise an error because '0' is already defined
    }
}

/**
# Enum Type Inference

Enums in Swag allow type inference, so you can often omit the enum type when assigning a value.
*/

/**
## Type Inference in Assignments

The enum type is not necessary in the assignment expression when declaring a variable, as it can be deduced.
*/
#test
{
    enum Values { A; B; }

    // The explicit form
    let x: Values = Values.A

    // The inferred form
    let y: Values = A

    @assert(x == y)
}

/**
## Type Inference in `switch` Statements

The enum type is not necessary in a `case` expression of a `switch` block, as it is inferred from the switch expression.
*/
#test
{
    enum Values { A; B; }
    let x = Values.A

    // The explicit form
    switch x
    {
    case Values.A: break
    case Values.B: break
    }

    // The simplified form
    switch x
    {
    case A: break
    case B: break
    }
}

/**
## Simplified Enum Syntax

In expressions where the enum type can be deduced, you can omit the enum name and use the `.Value` syntax.
*/
#test
{
    enum Values { A, B }

    // The explicit form
    let x = Values.A

    // The simplified form, since 'Values' can be inferred from 'x'
    @assert(x == .A)
    @assert(x != .B)
}

/**
## Simplified Syntax for Enum Flags

This also works for enums used as flags.
*/
#test
{
    #[Swag.EnumFlags]
    enum Values { A, B }

    let x = Values.A | Values.B
    @assert((x & .A) and (x & .B))
}

/**
## Simplified Enum Syntax in Function Calls

In most cases, the simplified `.Value` syntax also works when passing enum values to functions.
*/
#test
{
    enum Values { A; B; }
    func toto(v1, v2: Values) {}
    toto(.A, .B)
}

/**
# Visiting Enum Values

Using type reflection, Swag allows you to iterate over all the values of an enum. This feature is particularly useful when you need to perform operations across all enum values or when you want to dynamically generate behavior based on the values of an enum.

The two primary mechanisms for iterating over enum values are the `loop` construct and the `visit` statement.
*/

/**
## Looping Over Enum Values

The `loop` construct allows you to iterate over all values in an enum.
*/
#test
{
    enum RGB { R, G, B }                  // Define a simple enum with three values: R, G, and B

    // Counting Enum Values
    var cpt = 0
    loop idx in RGB:                      // Iterate over all enum values using 'loop'
        cpt += 1                          // Increment counter for each value
    @assert(cpt == 3)                     // Assert that the enum has exactly 3 values

    // Note that '@countof(RGB)' will give you the exact same result
    @assert(cpt == @countof(RGB))
}

/**
The `loop idx in RGB:` statement is a powerful construct that allows you to iterate over all values in the `RGB` enum. During each iteration, `idx` holds the current enum value, which can be used within the loop body. In the example above, the loop simply counts the number of values in the enum, asserting that the total is 3.

This method is useful when you need to apply the same operation to each enum value, such as populating a list, checking conditions, or performing calculations.
*/

/**
## Visiting Enum Values

The `visit` statement offers a more structured way to iterate over an enum and perform specific actions based on the current enum value.
*/
#test
{
    enum RGB { R, G, B }                  // Define a simple enum with three values: R, G, and B

    // Performing Actions Based on Enum Values
    visit val in RGB                      // Use 'visit' to iterate over each value in the enum
    {
        switch val                        // A switch statement to perform actions based on the enum value
        {
        case R:                           // Case for the value 'R'
            // Perform action for 'R'
            break
        case G:                           // Case for the value 'G'
            // Perform action for 'G'
            break
        case B:                           // Case for the value 'B'
            // Perform action for 'B'
            break
        default:                          
            // This should never be reached as we are covering all enum cases
            @assert(false)
        }
    }
}

/**
The `visit val in RGB` statement offers a more structured way to iterate over an enum. Each iteration provides the current enum value in the `val` variable, which can then be used in a `switch` statement (or other logic) to perform specific actions based on the value.

In the provided example, the `switch` statement checks the value of `val` and executes different blocks of code depending on whether `val` is `R`, `G`, or `B`. This is particularly useful when you need to perform different actions based on the specific value of the enum, rather than treating all values the same.
*/
