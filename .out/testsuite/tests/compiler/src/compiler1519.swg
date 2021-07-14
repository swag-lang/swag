struct A
{
    myTruc: u32 = 1
}

impl A
{
    #[Swag.macro]
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

//#[Swag.PrintBc]
#test
{
    var x: A
    x.toto()

    v := 0
}