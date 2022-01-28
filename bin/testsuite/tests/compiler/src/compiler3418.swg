//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    a := 666
    var v: closure(s32) = @|a|(x: s32)
    {
        //@print(a)
    }

    v(5)
}