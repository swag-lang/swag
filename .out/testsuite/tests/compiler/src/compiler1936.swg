//#[Swag.PrintBc]
#test
{
    a := 1's64
    b := 2's64
    c := a > b
    @assert(c == false)
    c1 := b > a
    @assert(c1 == true)
    d := a < b
    @assert(d == true)
    d1 := b < a
    @assert(d1 == false)
}

//#[Swag.PrintBc]
#test
{
    a := 1'f32
    b := 2'f32
    c := a > b
    @assert(c == false)
    c1 := b > a
    @assert(c1 == true)
    d := a < b
    @assert(d == true)
    d1 := b < a
    @assert(d1 == false)
}

//#[Swag.PrintBc]
#test
{
    a := 1'f64
    b := 2'f64
    c := a > b
    @assert(c == false)
    c1 := b > a
    @assert(c1 == true)
    d := a < b
    @assert(d == true)
    d1 := b < a
    @assert(d1 == false)
}