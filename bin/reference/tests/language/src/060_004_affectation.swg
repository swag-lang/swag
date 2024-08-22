/**
`opAffect` allows you to define custom assignment behavior for your struct using the `=` operator. You can overload `opAffect` to handle different types, providing specific behavior depending on the type of value assigned.
*/

struct Struct
{
    x, y, z: s32 = 666  // Fields with default values
}

impl Struct
{
    // Overloaded `opAffect` to handle `s32` assignments
    #[Swag.Overload]
    mtd opAffect(value: s32) { 
        x, y = value;  // Assign value to fields x and y
    }

    // Overloaded `opAffect` to handle `bool` assignments
    #[Swag.Overload]
    mtd opAffect(value: bool) { 
        x, y = value ? 1 : 0;  // Assign 1 if true, 0 if false, to fields x and y
    }
}

#test
{
    // Initialize 'v' and call opAffect(s32) with the value '4'
    var v: Struct = 4's32
    @assert(v.x == 4)    // 'x' is set to 4
    @assert(v.y == 4)    // 'y' is set to 4
    @assert(v.z == 666)  // 'z' retains its default value as 'opAffect' does not modify it

    // Initialize 'v1' and call opAffect(bool) with 'true'
    var v1: Struct = true
    @assert(v1.x == 1)   // 'x' is set to 1 (true)
    @assert(v1.y == 1)   // 'y' is set to 1 (true)

    // Assign 'false' to 'v1', invoking opAffect(bool)
    v1 = false
    @assert(v1.x == 0)   // 'x' is set to 0 (false)
    @assert(v1.y == 0)   // 'y' is set to 0 (false)
}

/**
If `opAffect` fully initializes the struct, you can use the `#[Swag.Complete]` attribute to optimize initialization. This prevents the struct from being initialized with default values before being reassigned, which can save performance.
*/

impl Struct
{
    // Marking `opAffect` with `Swag.Complete` ensures only one initialization step
    #[Swag.Complete, Swag.Overload]
    mtd opAffect(value: u64) { 
        x, y, z = cast(s32) value;  // Assign value to x, y, z, converting from u64 to s32
    }

    // Implicit conversion example for u16
    #[Swag.Implicit, Swag.Overload]
    mtd opAffect(value: u16) { 
        x, y = cast(s32) value;  // Assign value to x, y, converting from u16 to s32
    }
}

/**
Here, the variable `v` is initialized directly by `opAffect(u64)`, skipping the default initialization. This approach is more optimal as it reduces unnecessary assignments.
*/
#test
{
    var v: Struct = 2'u64
    @assert(v.x == 2)    // 'x' is directly set to 2
    @assert(v.y == 2)    // 'y' is directly set to 2
    @assert(v.z == 2)    // 'z' is directly set to 2
}

/**
By default, function arguments do not undergo automatic conversion via `opAffect`, so explicit casting is necessary.
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

    // Explicit cast required to invoke opAffect(s32)
    toto(cast(Struct) 5's32)

    // With #[Swag.Implicit], casting is not necessary; automatic conversion occurs
    toto(5'u16)  // Implicitly calls opAffect(u16)
}

/**
If you want a struct to be usable as a constant and initialized via `opAffect`, you can mark the method with `#[Swag.ConstExpr]`.
*/
struct Vector2
{
    x, y: f32  // Fields of type f32
}

impl Vector2
{
    #[Swag.ConstExpr]
    mtd opAffect(one: f32)
    {
        x, y = one  // Assign the same value to both x and y
    }
}

// Using opAffect(f32) to initialize a constant at compile time
const One: Vector2 = 1.0
#assert One.x == 1.0
#assert One.y == 1.0
