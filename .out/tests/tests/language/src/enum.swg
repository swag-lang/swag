#test
{
    // Enums values, unlike C, must end with ';' or an <eol>
    {
        enum Values
        {
            A
            B
        }
    }

    // By default, an enum is of type 's32'
    {
        using swag
        enum Values { A; B; }
        type := @typeof(Values)
        @assert(type.rawType == @typeof(s32))
    }

    // You can specify your own type
    {
        enum Value1: s64
        {
            A
            B
            C
        }
    }

    // Each value, if not specified, starts with 0 and increase at each new value
    {
        enum Value: s64
        {
            A
            B
            C
        }

        @assert(Value.A == 0)
        @assert(Value.B == 1)
        @assert(Value.C == 2)
    }

    // You can specify your own values
    {
        enum Value: s64
        {
            A = 10
            B = 20
            C = 30
        }

        @assert(Value.A == 10)
        @assert(Value.B == 20)
        @assert(Value.C == 30)
    }

    // If you omit one value, it will be the previous value + 1
    {
        enum Value: u32
        {
            A = 10
            B
            C
        }

        @assert(Value.A == 10)
        @assert(Value.B == 11)
        @assert(Value.C == 12)
    }

    // For non integer types, you must specify the values
    {
        enum Value1: string
        {
            A = "string 1"
            B = "string 2"
            C = "string 3"
        }

        @assert(Value1.A == "string 1")
        @assert(Value1.B == "string 2")
        @assert(Value1.C == "string 3")

        enum Value2: f32
        {
            A = 1.0
            B = 3.14
            C = 6
        }

        @assert(Value2.A == 1.0)
        @assert(Value2.B == 3.14)
        @assert(Value2.C == 6)
    }

    // '@countof' can be used on an enum
    {
        enum Value: string
        {
            A = "string 1"
            B = "string 2"
            C = "string 3"
        }

        @assert(@countof(Value) == 3)
        #assert(@countof(Value) == 3)
    }

    // You can use 'using' for an enum
    {
        enum Value
        {
            A
            B
            C
        }

        using Value
        @assert(A == 0)
        @assert(B == 1)
        @assert(C == 2)
        @assert(Value.A == 0)
        @assert(Value.B == 1)
        @assert(Value.C == 2)
    }

    // Enum can be a set of flags with the #[swag.enumflags] attribute
    // Type should be u8, u16, u32 or u64
    {
        #[swag.enumflags]
        enum MyFlags: u8
        {
            A
            B
            C
            D
        }

        @assert(MyFlags.A == 0b00000001)
        @assert(MyFlags.B == 0b00000010)
        @assert(MyFlags.C == 0b00000100)
        @assert(MyFlags.D == 0b00001000)

        value := MyFlags.B | MyFlags.C
        @assert(value == 0b00000110)
        @assert(value & MyFlags.B == MyFlags.B)
        @assert(value & MyFlags.C == MyFlags.C)
    }
}