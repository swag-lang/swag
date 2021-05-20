struct AA
{

}

impl AA
{
    //#[Swag.inline]
    #[Swag.implicit]
    func opCast(using self)->s64 => 666
}

//#[Swag.printbc]
func toto()->s64
{
    var x: AA
    return x
}

#test
{
    @assert(toto() == 666)
}