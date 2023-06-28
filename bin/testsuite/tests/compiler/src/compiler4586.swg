struct String
{
    val: string
}

impl String
{
    #[Swag.Implicit]
    mtd opAffect(x: string)
    {
        val = x
    }
}

//#[Swag.PrintBc, Swag.Optim("", false)]
func toto()->String
{
    return "toto"
}

#test
{
    a := toto()
    @assert(a.val == "toto")
}
