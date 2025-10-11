var G = 0

#[Swag.Overload]
func(T, N: u32) toto()
    where N <= 4
{
    G += 1
}

#[Swag.Overload]
func(T, N: u32) toto()
    where N > 4
{
    G += 10
}

#test
{
    toto'(f32, 4)()
    toto'(f32, 10)()
    @assert(G == 11)
}
