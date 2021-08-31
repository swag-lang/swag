#test
{
    // This is a struct declaration. 'var' is not necessary for the fields.
    {
        struct MyStruct1
        {
            name: string
        }

        struct MyStruct
        {
            x:      s32
            y, z:   s32
            val:    bool
            myS:    MyStruct1
        }
    }

    // You can initialize a struct variable in different ways
    {
        struct MyStruct
        {
            x, y: s32
        }

        // Default values
        var v0: MyStruct
        @assert(v0.x == 0)
        @assert(v0.y == 0)

        // Init in order of the fields
        var v1: MyStruct{10, 20}
        @assert(v1.x == 10)
        @assert(v1.y == 20)

        // Named field
        v2 := MyStruct{y: 20}
        @assert(v2.x == 0)
        @assert(v2.y == 20)

        // With a tuple
        var v3: MyStruct = @{10, 20}
        @assert(v3.x == 10)
        @assert(v3.y == 20)
    }

    // A struct can be initialized at the declaration
    {
        struct MyStruct
        {
            x: s32      = 666
            y: string   = "454"
        }

        v := MyStruct{}
        @assert(v.x == 666)
        @assert(v.y == "454")
    }

    // A struct can be affected to a constant, as long as it can be evaluated at compile time
    {
        struct MyStruct
        {
            x: s32      = 666
            y: string   = "454"
        }

        const X: MyStruct{50, "value"}
        #assert X.x == 50
        #assert X.y == "value"
    }

    // A struct can be anonymous when declared as a variable type
    // Unlike tuples, syntax should be the same as for named structs
    {
        var tuple: struct
        {
            x: f32
            y: f32
        }
        var tuple1: struct{x, y: f32; }

        tuple.x = 1.0
        tuple.y = 2.0
        @assert(tuple.x == 1.0)
        @assert(tuple.y == 2.0)
    }

    // You can force the layout of some fields with the 'Swag.Offset' attribute
    {
        struct MyStruct
        {
            x: s32

            // 'y' is located at the same offset as 'x', so they share the same space
            #[Swag.Offset("x")]
            y: s32
        }

        #assert @sizeof(MyStruct) == 4
        v := MyStruct{}
        v.x = 666
        @assert(v.y == 666)
    }

    {
        struct MyStruct
        {
            x, y, z:    f32

            #[Swag.Offset("x")]
            idx:        [3] f32
        }

        var v: MyStruct
        v.x = 10; v.y = 20; v.z = 30
        @assert(v.idx[0] == v.x)
        @assert(v.idx[1] == v.y)
        @assert(v.idx[2] == v.z)
    }

    // An union is just a struct where all fields are located at offset 0
    {
        union MyUnion
        {
            x, y, z: f32
        }

        v := MyUnion{x: 666}
        @assert(v.y == 666)
        @assert(v.z == 666)
    }
}
