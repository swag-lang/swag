var G = 0

#[Swag.Overload]
func(T) toto(x: *T)
{
    G += 1
}

#[Swag.Overload]
func(T) toto(x: const [..] T)
{
    G += 2
}

#test
{
    var a: s32
    toto(&a)
    toto(cast(const [..] s32) [1, 2])
    @assert(G == 3)
}
