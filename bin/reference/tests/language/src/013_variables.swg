/**
To declare a variable, use the `var` keyword, followed by `:` and then the type.
*/
#test
{
    {
        // Variable 'a' is of type 'u32' and its value is '1'.
        var a: u32 = 1
        @assert(a == 1)

        var b: string = "string"
        @assert(b == "string")
    }

    // We have already seen that we can declare multiple variables on the same line.
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
}

/**
If you don't assign a value, then the variable will be initialized with its default value (0). So a variable is **always** initialized.
*/
#test
{
    var a: bool
    @assert(a == false)

    var b: string
    @assert(b == null)

    var c: f64
    @assert(c == 0)
}

/**
But if you really do not want the variable to be initialized, you can assign it with `undefined`. To use with care, of course, but this is sometimes necessary to avoid the initialization cost.
*/
#test
{
    var a: bool = undefined
    var b: string = undefined
}

/**
We have seen that the type is optional in the declaration if it can be deduced from the assignment.
*/
#test
{
    // These are a bunch of type inferences.
    {
        var a = 1.5
        @assert(a == 1.5)
        #assert @typeof(a) == f32

        var b = "string"
        @assert(b == "string")
        #assert @typeof(b) == string

        var c = 1.5'f64
        @assert(c == 1.5)
        #assert @typeof(c) == f64
    }

    // The same goes for multiple variables.
    {
        var a, b = true
        @assert(a == true)
        @assert(b == true)
        #assert @typeof(a) == @typeof(true)
        #assert @typeof(b) == @typeof(a)

        var c = 1.5, d = "string"
        @assert(c == 1.5)
        @assert(d == "string")
        #assert @typeof(c) == f32
        #assert @typeof(d) == string
    }
}

/**
And in fact even `var` is optional if you use the short form with `:=`.
*/
#test
{
    a := 1.5
    @assert(a == 1.5)
    #assert @typeof(a) == f32

    b := "string"
    @assert(b == "string")
    #assert @typeof(b) == string

    c := 1.5'f64
    @assert(c == 1.5)
    #assert @typeof(c) == f64
}