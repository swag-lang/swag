struct AA
{
    l:   u64 = 4
}

impl AA
{
    #[Swag.Inline]
    func opCount(me)->u64 => me.l
}

//#[Swag.PrintBc]
//#[Swag.optimizebc(0)]
#test
{
    var x:  AA
    var cpt = 0
    for x do
        cpt += 1
    @assert(cpt == 4)
}
