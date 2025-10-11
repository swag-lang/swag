var G = 0

//#[Swag.PrintBc]
#[Swag.Overload]
func(T) toto(v: T)
    where #typeof(T) == f32
{
    #assert(#typeof(T) == f32)
    G += 1
}

#[Swag.Overload]
func(T) toto(v: T)
{
    G += 2
}

#test
{
    toto'f32(1.0)
    toto's32(1)
    @assert(G == 3)
}
