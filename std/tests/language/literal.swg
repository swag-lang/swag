#test
{
    // Integers in decimal, hexadecimal or binary forms
    {
        const a: u32 = 123456
        const b: u32 = 0xFFFF
        const c: u32 = 0b00001111
        @assert(a == 123456)
        @assert(b == 65535)
        @assert(c == 15)
    }

    // You can separate digits with the '_' character
    {
        const a: u32 = 123_456
        const b: u32 = 0xF_F_F_F
        const c: u32 = 0b0000_1111
        @assert(a == 123456)
        @assert(b == 65_535)
        @assert(c == 15)
    }

    // A boolean is 'true' or 'false'
    {
        const a = true
        #assert(a == true)
        const b, c = false
        #assert(b == false)
        #assert(c == false)
    }

    // A float value has the usual format
    {
        var a = 1.5
        @assert(a == 1.5)

        var b = 0.11
        @assert(b == 0.11)

        var c = 15e2
        @assert(c == 15e2)

        var d = 15e+2
        @assert(d == 15e2)

        var e = -1E-1
        @assert(e == -0.1)

    }

    // By default, a floating point value is 'f32', not 'f64' (aka double) like in c++
    {
        var a = 1.5
        @assert(a == 1.5)
        @assert(@typeof(a) == f32)
        @assert(@typeof(a) != f64)
    }

    // You can postfix a number by a type, to force it
    {
        var a = 1.5'f64
        @assert(a == 1.5)
        @assert(a == 1.5'f64)
        @assert(@typeof(a) == f64)

        b := 10'u8
        @assert(b == 10)
        @assert(@typeof(b) == u8)
    }
}