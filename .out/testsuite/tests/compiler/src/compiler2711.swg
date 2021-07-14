struct AA
{

}

impl AA
{
    #[Swag.Inline]
    func opCast(using self)->s64 => 666
}

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
func toto()->s64
{
    var x: AA
    return cast(s64) x
}

#test
{
    @assert(toto() == 666)
}