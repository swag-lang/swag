//#[Swag.PrintBc]
#test
{
    let a = 666
    func toto()
    {
        let a = 1
        func toto1()
        {
            let a = 2
        }
    }

    @assert(a == 666)
}
