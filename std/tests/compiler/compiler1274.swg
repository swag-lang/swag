//#[swag.printbc]
#test
{
    #[swag.macro]
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