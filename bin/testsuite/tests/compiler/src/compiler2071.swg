struct AA
{
    l: u64 = 4
}

impl AA
{
    #[Swag.Inline]
    func opCount(using self)->u64 => l
}

//#[Swag.PrintBc]
//#[Swag.optimizebc(0)]
#test
{
    var x: AA
    var cpt = 0
    loop x:
        cpt += 1
    @assert(cpt == 4)
}