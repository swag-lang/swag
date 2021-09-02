struct AA
{
    v: u32
}

func toto(y: s32, x: AA = @{5})
{
    @assert(x.v == 5)
}

//#[Swag.PrintBc]
#test
{
    toto(10)
}
