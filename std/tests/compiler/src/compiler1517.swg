struct A
{
    myTruc: s32
}

impl A
{
    #[swag.macro]
    func toto(using self)
    {
        #macro
        {
            myTruc = 666
        }
    }
}

//#[swag.printbc]
#test
{
    var x: A
    x.toto()
    @assert(x.myTruc == 666)
}