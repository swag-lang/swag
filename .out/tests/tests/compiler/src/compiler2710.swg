struct AA
{

}

impl AA
{
    //#[swag.inline]
    func opCast(using self)->s64 => 666
}

//#[swag.printbc]
func toto()->s64
{
    var x: AA
    return x
}

#test
{
    @assert(toto() == 666)
}