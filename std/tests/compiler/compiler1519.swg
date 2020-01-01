struct A
{
    myTruc: u32 = 1
}

impl A
{
    #[swag.macro]
    func toto(using self)
    {
        if !myTruc
            return

        //@print(cast(s64) myTruc)
        loop myTruc
        {
        }
    }
}

//#[swag.printbc]
#test
{
    var x: A
    x.toto()

    v := 0
}