struct A
{
    myTruc: u32 = 1
}

impl A
{
    #[Swag.Macro]
    func toto(using me)
    {
        if !myTruc:
            return

        //@print(cast(s64) myTruc)
        for myTruc
        {
        }
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A
    x.toto()

    var v = 0
}