alias alloc = func(u32)->*void
var cpt: u32 = 0

func toto(a: alloc)->*void
{
    return a(10)
}

func myAlloc(v: u32)->*void
{
    cpt = v
    return @alloc(cast(u64) v)
}

//#[Swag.PrintBc]
#test
{
    cpt = 0
    @free(toto(&myAlloc))
    @assert(cpt == 10)
}
