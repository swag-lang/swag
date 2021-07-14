struct AA
{

}

impl AA
{
    //#[Swag.inline]
    #[Swag.Implicit]
    func opCast(using self)->s64 => 666
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