struct AA
{

}

impl AA
{
    #[Swag.inline]
    func opCast(using self)->s64 => 666
}

//#[Swag.PrintBc]
//#[Swag.optim("", false)]
func toto()->s64
{
    var x: AA
    return cast(s64) x
}

#test
{
    @assert(toto() == 666)
}