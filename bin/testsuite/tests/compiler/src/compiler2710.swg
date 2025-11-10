struct AA {}

impl AA
{
    //#[Swag.Inline]
    #[Swag.Implicit]
    func opCast(me)->s64 => 666
}

//#[Swag.PrintBc]
func toto()->s64
{
    var x: AA
    return x
}

#test
{
    @assert(toto() == 666)
}
