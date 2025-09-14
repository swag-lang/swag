/**
# Custom Assignment Behavior with `opAffect`

The `opAffect` method in Swag allows you to define custom assignment behaviors for your struct using the `=` operator.
By overloading `opAffect`, you can handle assignments of different types, enabling you to specify how your struct should
behave when different types of values are assigned.
*/

struct Struct
{
    x, y, z: s32 = 666 // Fields 'x', 'y', and 'z' with a default value of 666
}

impl Struct
{
    // Overloaded `opAffect` to handle assignments of type `s32`
    #[Swag.Overload]
    mtd opAffect(value: s32)
    {
        me.x, me.y = value // Assign the provided `value` to both 'x' and 'y'
    }

    // Overloaded `opAffect` to handle assignments of type `bool`
    #[Swag.Overload]
    mtd opAffect(value: bool)
    {
        me.x, me.y = value ? 1 : 0 // Assign 1 if `value` is true, otherwise 0, to both 'x' and 'y'
    }
}

#test
{
    // Initialize 'v' and invoke `opAffect(s32)` with the value '4'
    let v: Struct = 4's32
    @assert(v.x == 4) // 'x' is set to 4
    @assert(v.y == 4) // 'y' is set to 4
    @assert(v.z == 666) // 'z' remains at its default value since `opAffect` doesn't modify it

    // Initialize 'v1' and invoke `opAffect(bool)` with 'true'
    var v1: Struct = true
    @assert(v1.x == 1) // 'x' is set to 1 because 'true' was assigned
    @assert(v1.y == 1) // 'y' is also set to 1 because 'true' was assigned

    // Assign 'false' to 'v1', triggering `opAffect(bool)`
    v1 = false
    @assert(v1.x == 0) // 'x' is set to 0 (false)
    @assert(v1.y == 0) // 'y' is set to 0 (false)
}

/**
# Optimizing Initialization with `Swag.Complete`

When `opAffect` fully initializes the struct, you can annotate it with `#[Swag.Complete]`. This prevents the struct from being initialized with default values before the assignment, enhancing performance by avoiding redundant assignments.
*/

impl Struct
{
    // Mark `opAffect` with `Swag.Complete` to ensure only one initialization step
    #[Swag.Complete, Swag.Overload]
    mtd opAffect(value: u64)
    {
        me.x, me.y, me.z = cast(s32) value // Convert `value` from u64 to s32 and assign it to 'x', 'y', and 'z'
    }

    // Implicit conversion example for u16 assignments
    #[Swag.Implicit, Swag.Overload]
    mtd opAffect(value: u16)
    {
        me.x, me.y = cast(s32) value // Convert `value` from u16 to s32 and assign it to 'x' and 'y'
    }
}

/**
In this case, the struct `v` is directly initialized by `opAffect(u64)`, skipping the default initialization step.
This optimization leads to more efficient code by reducing unnecessary initializations.
*/
#test
{
    let v: Struct = 2'u64
    @assert(v.x == 2) // 'x' is directly set to 2
    @assert(v.y == 2) // 'y' is directly set to 2
    @assert(v.z == 2) // 'z' is directly set to 2
}

/**
# Handling Function Arguments and Automatic Conversion

By default, function arguments do not automatically undergo conversion via `opAffect`. Explicit casting is necessary unless `Swag.Implicit` is used to allow automatic conversion.
*/
#test
{
    func toto(v: Struct)
    {
        @assert(v.x == 5)
        @assert(v.y == 5)
        @assert(v.z == 666) // 'z' remains unchanged
    }

    func titi(v: Struct)
    {
        @assert(v.y == 666) // 'y' remains at its default value
    }

    // Explicit cast required to invoke `opAffect(s32)`
    toto(cast(Struct) 5's32)

    // With `#[Swag.Implicit]`, casting is not required, and automatic conversion occurs
    toto(5'u16) // Implicitly calls `opAffect(u16)`
}

/**
# Using `opAffect` in Constant Expressions

To use `opAffect` in a context where the struct needs to be a constant, you can mark the method with `#[Swag.ConstExpr]`. This allows the struct to be initialized at compile-time via `opAffect`.
*/

struct Vector2
{
    x, y: f32 // Fields of type f32 representing coordinates
}

impl Vector2
{
    // Enables the use of `opAffect(f32)` for constant initialization
    #[Swag.ConstExpr]
    mtd opAffect(one: f32)
    {
        me.x, me.y = one
        // Assign the same value to both 'x' and 'y'
    }
}

// Using `opAffect(f32)` to initialize a compile-time constant
const One: Vector2 = 1.0
#assert(One.x == 1.0)
#assert(One.y == 1.0)
