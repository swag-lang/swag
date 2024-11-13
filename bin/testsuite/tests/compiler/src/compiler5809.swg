struct A 
{
    y: u32
}

impl A
{
    #[Swag.Implicit, Swag.ConstExpr]
    #[Swag.Optimize("", false)]
    mtd opAffect(x: u32)
    {
        y = x
    }

    #[Swag.Optimize("", false)]
    func from(x: u32)->A
    {
        return {x}
    }
}

const ColorYellow: A = 0xCAFECAFE

//#[Swag.PrintBc]
//#[Swag.Optimize("", false)]
func toto(b: bool)
{
    let a = b ? A.from(0x10) : ColorYellow
    @assert(a.y == 16)
}

#test
{
    toto(true)
}