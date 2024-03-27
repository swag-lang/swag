struct A
{

}

var G = 0
impl A
{
    func titi(using self)
    {
        G = 1
    }

    func toto(using self)
    {
        var fn: func(self)
        fn = &titi
        fn(self)
    }
}

#test
{
    var v: A
    v.toto()
    @assert(G == 1)
}