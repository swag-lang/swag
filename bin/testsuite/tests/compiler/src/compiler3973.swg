var G: s32

func toto(v: const ref s32)
{
    var ptr = &v
    @assert(@typeof(ptr) == const *s32)
    @assert(ptr == &G)
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    toto(&G)
}
