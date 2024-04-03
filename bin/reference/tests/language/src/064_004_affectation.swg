/**
`opAffect` is a way of assigning to a struct with `=`.
You can have more the one `opAffect` with different types.
*/

struct Struct
{
    x, y, z: s32 = 666
}

impl Struct
{
    #[Swag.Overload]
    mtd opAffect(value: s32) { x, y = value; }
    #[Swag.Overload]
    mtd opAffect(value: bool) { x, y = value ? 1 : 0; }
}

#test
{
    // This will initialize v and then call opAffect(s32) with '4'
    var v: Struct = 4's32
    @assert(v.x == 4)
    @assert(v.y == 4)
    // Note that variable 'v' is also initiliazed with the default values.
    // So here 'z' is still 666 because 'opAffect' does not change it.
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

/**
If `opAffect` is supposed to initialize the full content of the struct, you can add `#[Swag.Complete]`. This will avoid every variables to be initialized to the default values, then changed later with the `opAffect` call.
*/

impl Struct
{
    #[Swag.Complete, Swag.Overload]
    mtd opAffect(value: u64) { x, y, z = cast(s32) value; }

    // For later
    #[Swag.Implicit, Swag.Overload]
    mtd opAffect(value: u16) { x, y = cast(s32) value; }
}

/**
Here the variable `v` will not be initialized prior to the affectation. This is more optimal, as there's only one initialization.
*/
#test
{
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

    // By default, there's no automatic conversion for a function argument, so you must cast.
    // Here, this will create a 'Struct' on the stack, initialized with a call to 'opAffect(s32)'
    toto(cast(Struct) 5's32)

    // But if opAffect is marked with #[Swag.Implicit], automatic conversion can occur.
    // No need for an explicit cast.
    toto(5'u16)
}

/**
A structure can be converted to a constant by a call to `opAffect` if the function is marked with `Swag.ConstExpr`.
*/
struct Vector2
{
    x, y: f32
}

impl Vector2
{
    #[Swag.ConstExpr]
    mtd opAffect(one: f32)
    {
        x, y = one
    }
}

const One: Vector2 = 1.0 // This will call opAffect(1.0) at compile time
#assert One.x == 1.0
#assert One.y == 1.0
