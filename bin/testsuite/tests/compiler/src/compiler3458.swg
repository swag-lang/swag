//#global skip

var G = 0

#test
{
    func toto()
    {
        G = 10
    }

    var v = closure|&toto|() {
        toto()
    }

    v()
    @assert(G == 10)
}