struct AA
{
}

impl AA
{
    func toto() => 666
}

#test
{
    var x: @typeof(AA.toto)
    x = &AA.toto
    @assert(x() == 666)
}