struct AA
{

}

impl AA
{
    #[Swag.Inline]
    #[Swag.Implicit]
    func opCast(using self)->s64 => 666
}

//#[Swag.PrintBc]
//#[Swag.optim("", false)]
func toto()->s64
{
    var x: AA
    if true
        return x
    v := 10's64
    return v
}

#test
{
    @assert(toto() == 666)
}