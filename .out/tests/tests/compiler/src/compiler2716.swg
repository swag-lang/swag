struct AA
{

}

impl AA
{
    #[swag.inline]
    #[swag.implicit]
    func opCast(using self)->s64 => 666
}

//#[swag.printbc]
//#[swag.optim("", false)]
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