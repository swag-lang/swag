//#[Swag.printbc]
#test
{
    a := 666
    func toto()
    {
        a := 1
        func toto1()
        {
            a := 2
        }
    }

    @assert(a == 666)
}