struct AA
{
}

impl AA
{
    func toto() => 666
}

#test
{
    var x: @mktype(AA.toto)
    x = &AA.toto
    @assert(x() == 666)
}