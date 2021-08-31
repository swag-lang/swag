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

    {
        struct MyStruct
        {
            rgb: struct {x, y, z: f32; }
            hsl: struct {h, s, l: f32; }
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

    // The fields of a struct can be initialized at the declaration
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
}
