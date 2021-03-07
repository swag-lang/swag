struct AA
{
    v: u32
}

func toto(x: AA = @{5})
{
    @assert(x.v == 5)
}

//#[swag.printbc]
#test
{
    toto()
}
