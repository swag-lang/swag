/**
This is a 'struct' declaration.
Note that 'var' is not necessary for the fields.
*/
#test
{
    struct MyStruct
    {
        name: string
    }

    struct MyStruct1
    {
        x:       s32
        y, z:    s32
        val:     bool
        myS:     MyStruct
    }
}

/**
Variables can also be separated with ';' or ','. The last one is not mandatory.
*/
#test
{
    struct MyStruct
    {
        name:     string
        val1:     bool
    }
    struct MyStruct1
    {
        x:       s32
        y, z:    s32
        val:     bool
        myS:     MyStruct
    }
}

/**
A struct can be anonymous when declared as a variable type.
*/
#test
{
    var tuple: struct
    {
        x:     f32
        y:     f32
    }
    var tuple1: struct{x, y: f32}

    tuple.x = 1.0
    tuple.y = 2.0
    @assert(tuple.x == 1.0)
    @assert(tuple.y == 2.0)
}

#test
{
    struct MyStruct
    {
        rgb:     struct{x, y, z: f32}
        hsl:     struct{h, s, l: f32}
    }
}

/**
The fields of a struct can be initialized at the declaration.
*/
#test
{
    struct MyStruct
    {
        x:     s32 = 666
        y:     string = "454"
    }

    var v = MyStruct{}
    @assert(v.x == 666)
    @assert(v.y == "454")
}
#test
{
    // At fields declaration
    struct MyStruct
    {
        x, y: s32 = 1
    }

    // Without parameters, all fields will have the default values as defined
    // in the struct itself.
    var v0: MyStruct
    @assert(v0.x == 1)
    @assert(v0.y == 1)

    // You can add parameters between {..}
    // The initialization must be done in the order of the fields.
    var v1: MyStruct{10, 20}
    @assert(v1.x == 10)
    @assert(v1.y == 20)

    // You can name fields, and omit some.
    var v2 = MyStruct{y: 20}
    @assert(v2.x == 1)
    @assert(v2.y == 20)

    // You can assign a tuple as long as the types are correct.
    var v3: MyStruct = {10, 20}
    @assert(v3.x == 10)
    @assert(v3.y == 20)
}

/**
A struct can be affected to a constant, as long as it can be evaluated at compile time.
*/
#test
{
    struct MyStruct
    {
        x:     s32 = 666
        y:     string = "454"
    }

    const X: MyStruct{50, "value"}
    #assert X.x == 50
    #assert X.y == "value"
}

struct Struct3
{
    x, y, z: s32 = 666
}

func toto(v: Struct3)
{
    @assert(v.x == 5)
    @assert(v.y == 5)
    @assert(v.z == 666)
}

func titi(v: Struct3)
{
    @assert(v.x == 5)
    @assert(v.y == 666)
}

#test
{
    // Call with a struct literal
    toto(Struct3{5, 5, 666})

    // Type can be deduced from the argument
    toto({5, 5, 666})

    // You can also just specify some parts of the struct, in the declaration order of the fields
    titi({5}) // Initialize x, which is the first field
    titi({5, 666}) // Initialize x and y

    // You can also name the fields, and omit some of them
    titi({x: 5, z: 5}) // Here y will stay to the default value, which is 666
}
