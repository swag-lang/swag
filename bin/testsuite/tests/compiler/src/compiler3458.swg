//#global skip

var G = 0

#test
{
    func toto()
    {
        G = 10
    }

    v := closure|&toto|() {
        toto()
    }

    v()
    @assert(G == 10)
}