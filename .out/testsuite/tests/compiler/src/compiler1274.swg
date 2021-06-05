//#[Swag.printbc]
#test
{
    #[Swag.macro]
    func toto()
    {
        `a += 2
    }

    a := 0
    toto()
    toto()
    toto()
    @assert(a == 6)
}