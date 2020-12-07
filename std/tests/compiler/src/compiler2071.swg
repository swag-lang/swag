struct AA
{
    l: u32 = 4
}

impl AA
{
    #[swag.inline]
    func opCount(using self)->u32 => l
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