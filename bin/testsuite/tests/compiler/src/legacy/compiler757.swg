#[Swag.ConstExpr]
func toto()->s32
{
    return 666
}

func toto1(x: s32) {}

//#[Swag.PrintBc]
#test
{
    //@print(toto)
    var x = 0
    x = toto()
    @assert(x == 666)
}
