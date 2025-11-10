var G = 0

#[Swag.Overload]
func(T) toto()
    where #typeof(T) == f32
{
    G += 1
}

#[Swag.Overload]
func(T) toto()
    where #typeof(T) == s32
{
    G += 2
}

#test
{
    toto's32()
    toto'f32()
    @assert(G == 3)
}
