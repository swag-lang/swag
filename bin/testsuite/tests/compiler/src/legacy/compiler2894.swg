struct A {}

var G = 0
impl A
{
    func titi(me)
    {
        G = 1
    }

    func toto(me)
    {
        var fn: func(*A)
        fn = &titi
        fn(me)
    }
}

#test
{
    var v: A
    v.toto()
    @assert(G == 1)
}
