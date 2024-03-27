//#[Swag.PrintBc]
#test
{
    #[Swag.Macro]
    func toto()
    {
        #up a += 2
    }

    var a = 0
    toto()
    toto()
    toto()
    @assert(a == 6)
}
