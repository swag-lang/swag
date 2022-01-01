struct Struct
{
    x, y, z: s32 = 666
}

impl Struct
{
    // opAffect is a way of assigning to a struct with '='
    // You can have more the one opAffect with different types
    mtd opAffect(value: s32)  { x, y = value; }
    mtd opAffect(value: bool) { x, y = value ? 1 : 0; }
}

#test
{
    // This will initialize v and then call opAffect(s32) with '4'
    var v: Struct = 4's32
    @assert(v.x == 4)
    @assert(v.y == 4)
    // Note that variable 'v' is also initiliazed with the default values.
    // So here 'z' is still 666.
    @assert(v.z == 666)

    // This will call opAffect(bool) with 'true'
    var v1: Struct = true
    @assert(v1.x == 1)
    @assert(v1.y == 1)

    // This will call opAffect(bool) with 'false'
    v1 = false
    @assert(v1.x == 0)
    @assert(v1.y == 0)
}

impl Struct
{
    // If opAffect is supposed to initialize the full content of the struct, you can add @[Swag.Complete]
    // This will avoid a variable to be initialized to its default values, then changed later
    // with the 'opAffect' call.
    #[Swag.Complete]
    mtd opAffect(value: u64)  { x, y, z = cast(s32) value; }

    // For later
    #[Swag.Implicit]
    mtd opAffect(value: u16)  { x, y = cast(s32) value; }
}

#test
{
    // Here the variable 'v' will not be initialized prior to the affectation
    // This is more optimal, as there's only one initialization
    var v: Struct = 2'u64
    @assert(v.x == 2)
    @assert(v.y == 2)
    @assert(v.z == 2)
}

#test
{
    func toto(v: Struct)
    {
        @assert(v.x == 5)
        @assert(v.y == 5)
        @assert(v.z == 666)
    }

    func titi(v: Struct)
    {
        @assert(v.y == 666)
    }

    // By default, there's no automatic conversion for a function argument, so you must cast
    toto(cast(Struct) 5's32)

    // But if opAffect is marked with #[Swag.Implicit], automatic conversion can occur
    toto(5'u16)

    // Call with a struct literal
    toto(Struct{5, 5, 666})

    // Type can be deduced from the argument
    toto(@{5, 5, 666})

    // You can also just specify some parts of the struct, in the declaration order of the fields
    titi(@{5})      // Initialize x, which is the first field
    titi(@{5, 666}) // Initialize x and y

    // You can also name the fields, and omit some of them
    titi(@{x: 5, z: 5}) // Here y will stay to the default value, which is 666
}