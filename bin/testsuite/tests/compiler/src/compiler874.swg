//#[Swag.PrintBc]
#test
{
    let square = func(x: f32)->f32 { return x * x; }
    let v      = square(2)
    @assert(v == 4)
}
