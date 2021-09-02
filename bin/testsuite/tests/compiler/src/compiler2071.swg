struct AA
{
    l: uint = 4
}

impl AA
{
    #[Swag.Inline]
    func opCount(using self)->uint => l
}

//#[Swag.PrintBc]
//#[Swag.optimizebc(0)]
#test
{
    var x: AA
    cpt := 0
    loop x
        cpt += 1
    @assert(cpt == 4)
}