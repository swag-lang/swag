struct A
{
    myTruc: s32
}

impl A
{
    #[Swag.Macro]
    func toto(using self)
    {
        #macro
        {
            `self.myTruc = 666
        }
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A
    x.toto()
    @assert(x.myTruc == 666)
}