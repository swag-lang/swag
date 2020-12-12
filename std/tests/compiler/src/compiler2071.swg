struct AA
{
    l: uint = 4
}

impl AA
{
    #[swag.inline]
    func opCount(using self)->uint => l
}

//#[swag.printbc]
//#[swag.optimizebc(0)]
#test
{
    var x: AA
    cpt := 0
    loop x
        cpt += 1
    @assert(cpt == 4)
}