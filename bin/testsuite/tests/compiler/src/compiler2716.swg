struct AA
{

}

impl AA
{
    #[Swag.Inline]
    #[Swag.Implicit]
    func opCast(using me)->s64 => 666
}

//#[Swag.PrintBc]
//#[Swag.Optimize("", false)]
func toto()->s64
{
    var x: AA
    if true:
        return x
    let v = 10's64
    return v
}

#test
{
    @assert(toto() == 666)
}