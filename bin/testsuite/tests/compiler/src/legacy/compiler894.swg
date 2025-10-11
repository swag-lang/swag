func toto(x: { a: f32 })
{
    @assert(x.item0 == 1.0)
}

//#[Swag.PrintBc]
#test
{
    toto({1.0})
}
