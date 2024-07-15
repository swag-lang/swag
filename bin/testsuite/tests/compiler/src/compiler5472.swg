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
#[Swag.Inline]
func toto()->String
{
    return "toto"
}

#test
{
    var a = toto()
    @assert(a.val == "toto")
}
