//#[Swag.PrintBc]
#test
{
    let a = 1's64
    let b = 2's64
    let c = a > b
    @assert(c == false)
    let c1 = b > a
    @assert(c1 == true)
    let d = a < b
    @assert(d == true)
    let d1 = b < a
    @assert(d1 == false)
}

//#[Swag.PrintBc]
#test
{
    let a = 1'f32
    let b = 2'f32
    let c = a > b
    @assert(c == false)
    let c1 = b > a
    @assert(c1 == true)
    let d = a < b
    @assert(d == true)
    let d1 = b < a
    @assert(d1 == false)
}

//#[Swag.PrintBc]
#test
{
    let a = 1'f64
    let b = 2'f64
    let c = a > b
    @assert(c == false)
    let c1 = b > a
    @assert(c1 == true)
    let d = a < b
    @assert(d == true)
    let d1 = b < a
    @assert(d1 == false)
}
