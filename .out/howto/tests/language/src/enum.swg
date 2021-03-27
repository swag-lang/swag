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

    // @kindof will return the underlying type of the enum
    {
        enum RGB: u32 { R; G; B; }
        @assert(@typeof(RGB) == RGB)
        @assert(@kindof(RGB) != RGB)
        @assert(@kindof(RGB) == u32)
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

    // You can have an enum of static arrays
    {
        enum Value: const [2] s32
        {
            A = @[1, 2]
            B = @[10, 20]
        }

        #assert Value.A[0] == 1
        #assert Value.A[1] == 2
        #assert Value.B[0] == 10
        #assert Value.B[1] == 20
    }

    // You can have an enum of const slices
    {
        enum Value: const [..] s32
        {
            A = @[1, 2]
            B = @[10, 20, 30, 40]
        }

        #assert @countof(Value.A) == 2
        #assert @countof(Value.B) == 4
        x := Value.A
        @assert(x[0] == 1)
        @assert(x[1] == 2)
    }

    // A variable of type enum must be initialized, except if ? is used
    {
        enum Values { A; B; }
        var x: Values = ?
        var y: Values = Values.A
    }

    // The type of the enum is not necessary on the assignement expression
    {
        enum Values { A; B; }
        var x: Values = Values.A
        var y: Values = A
        @assert(x == y)
    }

    // The enum type is not necessary in a case expression of a switch block
    {
        enum Values { A; B; }
        x := Values.A
        switch x
        {
        case A: break
        case B: break
        }

        switch x
        {
        case Values.A: break
        case Values.B: break
        }
    }
}