/**
Enums values, unlike C/C++, can end with `;` or `,` or an `eol`.
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
By default, an enum is of type `s32`.
*/
#test
{
    enum Values { A, B }
    let type = @typeof(Values)

    // 'type' here is a 'typeinfo' dedicated to the enum type.
    // In that case, 'type.rawType' is the enum underlying type.
    @assert(type.rawType == s32)
    #assert @typeof(Values) == Values
}

/**
`@kindof` will return the underlying type of the enum.
*/
#test
{
    enum RGB { R, G, B }
    #assert @typeof(RGB) == RGB
    #assert @kindof(RGB) != RGB
    #assert @kindof(RGB) == s32
}

/**
You can specify your own type after the enum name.
*/
#test
{
    enum Values1: s64 // Forced to be 's64' instead of 's32'
    {
        A
        B
        C
    }

    #assert @kindof(Values1) == s64
    #assert @typeof(Values1.A) == Values1
}

/**
Enum values, if not specified, start at 0 and are increased by 1 at each new value.
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
You can specify your own values.
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
If you omit one value, it will be assigned to the previous value + 1.
*/
#test
{
    enum Value: u32
    {
        A = 10
        B // 11
        C // 12
    }

    #assert Value.A == 10
    #assert Value.B == 11
    #assert Value.C == 12
}

/**
For non integer types, you **must** specify the values as they cannot be deduced.
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
        C = 6
    }

    #assert Value2.A == 1.0
    #assert Value2.B == 3.14
    #assert Value2.C == 6
}

/**
`@countof` returns the number of values of an enum.
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
You can use the keyword `using` for an enum.
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

    // No need to say 'Value.A' thanks to the 'using' above
    @assert(A == 0)
    @assert(B == 1)
    @assert(C == 2)

    // But the normal form is still possible
    @assert(Value.A == 0)
    @assert(Value.B == 1)
    @assert(Value.C == 2)
}


/**
# Enum as flags
An enum can be a set of flags if you declare it with the `#[Swag.EnumFlags]` **attribute**. Its type should be `u8`, `u16`, `u32` or `u64`.

That kind of enum starts by default at 1, and not 0, and each value should be a power of 2.
*/
#test
{
    #[Swag.EnumFlags]
    enum MyFlags: u8
    {
        A   // First value is 1 and *not* 0
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
# Enum of arrays
You can have an enum of const static arrays.
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
# Enum of slices
You can have an enum of const slices.
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
# Nested enums
*/

/**
An enum can be nested inside another enum with 'using'. Both enums must of course have the same underlying type.
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
    NotFound
}

/**
To access a value inside the nested enum, use the enum name (a scope has been created).
*/
const MyError0 = MyErrors.BasicErrors.FailedToSave

/**
An **automatic cast** will be done if you try to convert a nested enum to an owner enum.
For example, a value of type 'BasicErrors' can be converted to a parameter of type 'MyErrors' because 'MyErrors' contains 'BasicErrors'.
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
# Enum type inference
*/

/**
The type of the enum is not necessary in the assignement expression when declaring a variable.
*/
#test
{
    enum Values { A; B; }

    // The normal form
    let x: Values = Values.A

    // But in fact 'Values' is not necessary because it can be deduced
    let y: Values = A

    @assert(x == y)
}

/**
The enum type is not necessary in a `case` expression of a `switch` block (it is deduced from the switch expression).
*/
#test
{
    enum Values { A; B; }
    let x = Values.A

    // The 'normal' form
    switch x
    {
    case Values.A: break
    case Values.B: break
    }

    // The 'simplified' form
    switch x
    {
    case A: break
    case B: break
    }
}

/**
In an expression, and if the enum name can be deduced, you can omit it and use the `.Value` syntax.
*/
#test
{
    enum Values { A, B }

    // The normal form
    let x = Values.A

    // The simplified form, because 'Values' can be deduced from type of x
    @assert(x == .A)
    @assert(x != .B)
}

/**
Works also for flags.
*/
#test
{
    #[Swag.EnumFlags]
    enum Values { A, B }

    let x = Values.A | Values.B
    @assert((x & .A) and (x & .B))
}

/**
Works also (most of the time), for functions.
*/
#test
{
    enum Values { A; B; }
    func toto(v1, v2: Values) {}
    toto(.A, .B)
}


/**
By type reflection, you can loop/visit all values of an enum.
*/
#test
{
    {
        enum RGB { R, G, B }

        var cpt = 0
        loop idx: RGB
            cpt += 1
        @assert(cpt == 3)

        visit val: RGB
        {
            switch val
            {
            case R:     break
            case G:     break
            case B:     break
            default:    @assert(false)
            }
        }
    }
}