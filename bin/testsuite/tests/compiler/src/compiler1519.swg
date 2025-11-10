struct A
{
    myTruc: u32 = 1
}

impl A
{
    #[Swag.Macro]
    func toto(me)
    {
        if !me.myTruc do
            return

        //@print(cast(s64) myTruc)
        for me.myTruc
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
