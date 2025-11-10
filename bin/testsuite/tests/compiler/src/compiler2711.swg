struct AA {}

impl AA
{
    #[Swag.Inline]
    func opCast(me)->s64 => 666
}

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
func toto()->s64
{
    var x: AA
    return cast(s64) x
}

#test
{
    @assert(toto() == 666)
}
