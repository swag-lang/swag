//#[Swag.PrintBc]
#test
{
    var a = 666
    func toto()
    {
        var a = 1
        func toto1()
        {
            var a = 2
        }
    }

    @assert(a == 666)
}