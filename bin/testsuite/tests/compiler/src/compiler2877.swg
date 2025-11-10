struct AA {}

impl AA
{
    func toto() => 666
}

#test
{
    var x: #decltype(AA.toto)
    x = &AA.toto
    @assert(x() == 666)
}
