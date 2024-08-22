/**
This is a basic 'struct' declaration.
Note that 'var' is not required for declaring fields within the struct.
*/
#test
{
    struct MyStruct
    {
        name: string  // Field 'name' of type 'string'
    }

    struct MyStruct1
    {
        x:    s32     // Field 'x' of type 's32'
        y, z: s32     // Fields 'y' and 'z' of type 's32', declared together
        val:  bool    // Field 'val' of type 'bool'
        myS:  MyStruct // Field 'myS' of type 'MyStruct', demonstrating a struct within a struct
    }
}

/**
Fields within a struct can be separated by either ';' or ','.
The trailing separator is optional.
*/
#test
{
    struct MyStruct { name: string, val1: bool }  // Fields separated by commas
    struct MyStruct1 { x: s32; y, z: s32; val: bool; myS: MyStruct }  // Fields separated by semicolons
}

/**
A struct can be anonymous when declared as a variable type.
*/
#test
{
    var tuple: struct
    {
        x: f32    // Field 'x' of type 'f32'
        y: f32    // Field 'y' of type 'f32'
    }

    var tuple1: struct{ x, y: f32 }  // Anonymous struct with fields 'x' and 'y' of type 'f32'

    tuple.x = 1.0  // Assigning value to 'x'
    tuple.y = 2.0  // Assigning value to 'y'
    @assert(tuple.x == 1.0)  // Asserting 'x' is 1.0
    @assert(tuple.y == 2.0)  // Asserting 'y' is 2.0
}

#test
{
    struct MyStruct
    {
        rgb: struct{ x, y, z: f32 }  // Nested anonymous struct for RGB values
        hsl: struct{ h, s, l: f32 }  // Nested anonymous struct for HSL values
    }
}

/**
Struct fields can be initialized with default values.
*/
#test
{
    struct MyStruct
    {
        x: s32    = 666     // Field 'x' with default value 666
        y: string = "454"   // Field 'y' with default value "454"
    }

    var v = MyStruct{}  // Initializing struct with default values
    @assert(v.x == 666)  // Asserting 'x' has the default value 666
    @assert(v.y == "454")  // Asserting 'y' has the default value "454"
}

/**
Struct variables can be initialized in different ways.
*/
#test
{
    // Initializing fields within the struct declaration
    struct MyStruct
    {
        x, y: s32 = 1  // Both 'x' and 'y' are initialized to 1
    }

    // Without parameters, all fields take their default values as defined in the struct
    var v0: MyStruct  // Struct with default initialization
    @assert(v0.x == 1)  // Asserting 'x' is 1
    @assert(v0.y == 1)  // Asserting 'y' is 1

    // Initializing with parameters between {..}, in the order of field declarations
    var v1: MyStruct{10, 20}  // Initializing 'x' with 10 and 'y' with 20
    @assert(v1.x == 10)  // Asserting 'x' is 10
    @assert(v1.y == 20)  // Asserting 'y' is 20

    // Named initialization of fields, allowing omission of some fields
    var v2 = MyStruct{y: 20}  // 'x' takes the default value, 'y' is initialized to 20
    @assert(v2.x == 1)  // Asserting 'x' is the default value 1
    @assert(v2.y == 20)  // Asserting 'y' is 20

    // Initializing using a tuple, as long as types match
    var v3: MyStruct = {10, 20}  // Initializing using a tuple
    @assert(v3.x == 10)  // Asserting 'x' is 10
    @assert(v3.y == 20)  // Asserting 'y' is 20
}

/**
A struct can be assigned to a constant, as long as it can be evaluated at compile-time.
*/
#test
{
    struct MyStruct
    {
        x: s32    = 666     // Field 'x' with default value 666
        y: string = "454"   // Field 'y' with default value "454"
    }

    const X: MyStruct{50, "value"}  // Constant struct with specific values
    #assert X.x == 50  // Compile-time assertion that 'x' is 50
    #assert X.y == "value"  // Compile-time assertion that 'y' is "value"
}

/**
A function can take a struct as an argument.
No copy is made; this is equivalent to a const reference in C++.
*/

struct Struct3
{
    x, y, z: s32 = 666  // Struct with default values for 'x', 'y', and 'z'
}

func toto(v: Struct3)
{
    @assert(v.x == 5)  // Asserting 'x' is 5
    @assert(v.y == 5)  // Asserting 'y' is 5
    @assert(v.z == 666)  // Asserting 'z' is the default value 666
}

func titi(v: Struct3)
{
    @assert(v.x == 5)  // Asserting 'x' is 5
    @assert(v.y == 666)  // Asserting 'y' is the default value 666
}

#test
{
    // Calling with a struct literal
    toto(Struct3{5, 5, 666})  // Passing explicit values for 'x', 'y', and 'z'

    // Type can be inferred from the argument
    toto({5, 5, 666})  // Inferring the struct type from the provided values

    // Initializing only some parts of the struct, in the order of field declarations
    titi({5})  // Initializes 'x' only, 'y' and 'z' take default values
    titi({5, 666})  // Initializes 'x' and 'y', 'z' takes the default value

    // Initializing using named fields, with the ability to omit some fields
    titi({x: 5, z: 5})  // 'y' remains at its default value of 666, 'x' and 'z' are set
}
