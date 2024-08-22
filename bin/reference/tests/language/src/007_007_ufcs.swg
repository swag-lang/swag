/**
# Introduction to Uniform Function Call Syntax (UFCS)
*UFCS* stands for *uniform function call syntax*. It allows any function to be called using the `param.func()` form when the first parameter of `func()` matches the type of `param`.
This syntax provides a way to call static functions as if they were methods on an instance.
*/

#test
{
    func myFunc(param: bool) => param

    let b = false
    @assert(myFunc(b) == b.myFunc()) // Using UFCS to call 'myFunc' as if it were a method on 'b'
}

/**
# Static Functions as Methods
In Swag, all functions are *static*, meaning they are not bound to instances of structs or classes. However, UFCS allows these functions to be called in a method-like style.
This feature is particularly useful for struct manipulation, making the code more readable and intuitive.
*/
#test
{
    struct Point { x, y: s32 }

    func set(using pt: *Point, value: s32)
    {
        x, y = value
    }

    var pt: Point

    // Using UFCS to call 'set' as if it were a method of 'pt'
    pt.set(10)
    @assert(pt.x == 10 and pt.y == 10)

    // Normal static function call
    set(&pt, 20)
    @assert(pt.x == 20 and pt.y == 20)
}

/**
# UFCS with Multiple Parameters
UFCS works seamlessly with functions that take multiple parameters, provided the first parameter matches the type of the instance.
*/
#test
{
    struct Vector { x, y: f32 }

    func add(using vec: *Vector, dx: f32, dy: f32)
    {
        x += dx
        y += dy
    }

    var v: Vector

    // Using UFCS to call 'add' as if it were a method of 'v'
    v.add(1.0, 2.0)
    @assert(v.x == 1.0 and v.y == 2.0)

    // Normal static function call
    add(&v, 3.0, 4.0)
    @assert(v.x == 4.0 and v.y == 6.0)
}

/**
# UFCS and Function Overloading
UFCS supports function overloading, where the appropriate function is chosen based on the types of the parameters provided.
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
    func multiply(using c: *Complex, other: *Complex)
    {
        real = (real * other.real) - (imag * other.imag)
        imag = (real * other.imag) + (imag * other.real)
    }

    var c1 = Complex{2.0, 3.0}
    var c2 = Complex{4.0, 5.0}

    // Using UFCS to multiply by a scalar
    c1.multiply(2.0)
    @assert(c1.real == 4.0 and c1.imag == 6.0)

    // Using UFCS to multiply by another Complex number
    c1.multiply(&c2)
    @assert(c1.real == -14.0 and c1.imag == -46.0)
}
