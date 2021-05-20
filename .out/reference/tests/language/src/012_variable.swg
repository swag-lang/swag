#test
{
    // To declare a variable, use the 'var' keyword
    {
        var a: u32 = 1
        @assert(a == 1)

        var b: string = "string"
        @assert(b == "string")
    }

    // You can also declare multiple variables on the same line
    {
        var a, b: u32 = 123
        @assert(a == 123)
        @assert(b == 123)
    }

    // Or
    {
        var a: u32 = 12, b: f32 = 1.5
        @assert(a == 12)
        @assert(b == 1.5)
    }

    // If you don't assign a value, then the variable will be initialized with its default value
    {
        var a: bool
        @assert(a == false)

        var b: string
        @assert(b == null)

        var c: f64
        @assert(c == 0)
    }

    // If you do not want the variable to be initialized, you can
    {
        var a: bool = ?
        var b: string = ?
    }

    // Type is optional if it can be deduced from the assignment
    {
        var a = 1.5
        @assert(a == 1.5)
        @assert(@typeof(a) == f32)

        var b = "string"
        @assert(b == "string")
        @assert(@typeof(b) == string)

        var c = 1.5'f64
        @assert(c == 1.5)
        @assert(@typeof(c) == f64)
    }

    // Same for multiple variables
    {
        var a, b = true
        @assert(a == true)
        @assert(b == true)
        @assert(@typeof(a) == @typeof(true))
        @assert(@typeof(b) == @typeof(a))

        var c = 1.5, d = "string"
        @assert(c == 1.5)
        @assert(d == "string")
        @assert(@typeof(c) == f32)
        @assert(@typeof(d) == string)
    }

    // 'var' is optional if you use ':='
    {
        a := 1.5
        @assert(a == 1.5)
        @assert(@typeof(a) == f32)

        b := "string"
        @assert(b == "string")
        @assert(@typeof(b) == string)

        c := 1.5'f64
        @assert(c == 1.5)
        @assert(@typeof(c) == f64)
    }

    // If you use 'const' instead of 'var', the value must be known by the compiler
    // (no memory footprint)
    {
        const a = 666
        #assert(a == 666)

        const b: string = "string"
        #assert(b == "string")
    }

    // You can use '@typeof' or '@kindof' to get the type of another expression
    {
        var a = 0
        var b: @typeof(a) = 1
        @assert(@typeof(a) == @typeof(b))
    }

    // A local variable declared with the #[Swag.global] attribute will be global
    {
        func toto()->s32
        {
            #[Swag.global] var x = 1
            x += 1
            return x
        }

        @assert(toto() == 2)
        @assert(toto() == 3)
        @assert(toto() == 4)
    }
}