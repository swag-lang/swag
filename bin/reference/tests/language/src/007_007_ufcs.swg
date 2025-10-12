/**
# Introduction to Uniform Function Call Syntax (UFCS)
*UFCS* (Uniform Function Call Syntax) allows a function to be called in the `param.func()` form
when the first parameter type of `func()` matches `param`.
This enables calling standalone functions as if they were instance methods, improving readability.
*/
#test
{
    func myFunc(param: bool) => param

    let b = false
    @assert(myFunc(b) == b.myFunc()) // UFCS allows method-like syntax
}

/**
# Static Functions as Methods
All functions in Swag are static, but UFCS enables them to be invoked with instance-style syntax.
This improves clarity when working with structs or objects.
*/
#test
{
    struct Point { x, y: s32 }

    func set(pt: *Point, value: s32)
    {
        pt.x, pt.y = value
    }

    var pt: Point

    // UFCS — called like a method
    pt.set(10)
    @assert(pt.x == 10 and pt.y == 10)

    // Normal static function call
    set(&pt, 20)
    @assert(pt.x == 20 and pt.y == 20)
}

/**
# UFCS with Multiple Parameters
UFCS works with multi-parameter functions as long as the first parameter type matches the instance type.
*/
#test
{
    struct Vector { x, y: f32 }

    func add(vec: *Vector, dx: f32, dy: f32)
    {
        vec.x += dx
        vec.y += dy
    }

    var v: Vector

    // UFCS style
    v.add(1.0, 2.0)
    @assert(v.x == 1.0 and v.y == 2.0)

    // Standard function call
    add(&v, 3.0, 4.0)
    @assert(v.x == 4.0 and v.y == 6.0)
}

/**
# UFCS and Function Overloading
UFCS supports overloaded functions, selecting the correct overload based on argument types.
*/
#test
{
    struct Complex { real, imag: f32 }

    #[Swag.Overload]
    func multiply(c: *Complex, scalar: f32)
    {
        c.real *= scalar
        c.imag *= scalar
    }

    #[Swag.Overload]
    func multiply(c: *Complex, other: *Complex)
    {
        // Use temporary variables to prevent reuse of modified values
        let r = (c.real * other.real) - (c.imag * other.imag)
        let i = (c.real * other.imag) + (c.imag * other.real)
        c.real = r
        c.imag = i
    }

    var c1 = Complex{2.0, 3.0}
    var c2 = Complex{4.0, 5.0}

    // UFCS: multiply by scalar
    c1.multiply(2.0)
    @assert(c1.real == 4.0 and c1.imag == 6.0)

    // UFCS: multiply by another complex number
    c1.multiply(&c2)
    @assert(c1.real == -14.0 and c1.imag == 44.0)
}
