struct A
{

}

var G = 0
impl A
{
    func titi(using me)
    {
        G = 1
    }

    func toto(using me)
    {
        var fn: func(me)
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