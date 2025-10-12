/**
# Custom Assignment Behavior with 'opAffect'

The 'opAffect' method in Swag allows you to define custom assignment behaviors
for your struct using the '=' operator. By overloading 'opAffect', you can handle
assignments of different types and control how your struct responds.
*/

struct Struct
{
    x, y, z: s32 = 666 // Fields with default value 666
}

impl Struct
{
    // Overload for 's32'
    #[Swag.Overload]
    mtd opAffect(value: s32)
    {
        me.x, me.y = value
    }

    // Overload for 'bool'
    #[Swag.Overload]
    mtd opAffect(value: bool)
    {
        me.x, me.y = value ? 1 : 0
    }
}

#test
{
    let v: Struct = 4's32
    @assert(v.x == 4)
    @assert(v.y == 4)
    @assert(v.z == 666)

    var v1: Struct = true
    @assert(v1.x == 1)
    @assert(v1.y == 1)

    v1 = false
    @assert(v1.x == 0)
    @assert(v1.y == 0)
}

/**
# Optimizing Initialization with 'Swag.Complete'

When 'opAffect' completely initializes the struct, mark it with '#[Swag.Complete]'.
This avoids default initialization before assignment for better performance.
*/

impl Struct
{
    #[Swag.Complete, Swag.Overload]
    mtd opAffect(value: u64)
    {
        me.x, me.y, me.z = cast(s32) value
    }

    #[Swag.Implicit, Swag.Overload]
    mtd opAffect(value: u16)
    {
        me.x, me.y = cast(s32) value
    }
}

#test
{
    let v: Struct = 2'u64
    @assert(v.x == 2)
    @assert(v.y == 2)
    @assert(v.z == 2)
}

/**
# Handling Function Arguments and Automatic Conversion

Function arguments are not automatically converted through 'opAffect' unless
'Swag.Implicit' is used. Otherwise, an explicit cast is required.
*/

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

    // Explicit cast triggers 'opAffect(s32)'
    toto(cast(Struct) 5's32)

    // Implicit conversion via 'opAffect(u16)'
    toto(5'u16)
}

/**
# Using 'opAffect' in Constant Expressions

To allow compile-time initialization through 'opAffect', mark it with
'#[Swag.ConstExpr]'.
*/

struct Vector2 { x, y: f32 }

impl Vector2
{
    #[Swag.ConstExpr]
    mtd opAffect(one: f32)
    {
        me.x, me.y = one
    }
}

const One: Vector2 = 1.0
#assert(One.x == 1.0)
#assert(One.y == 1.0)
