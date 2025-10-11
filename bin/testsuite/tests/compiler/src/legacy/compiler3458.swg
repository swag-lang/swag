//#global skip

var G = 0

#test
{
    func toto()
    {
        G = 10
    }

    let v = func|&toto|()
    {
        toto()
    }

    v()
    @assert(G == 10)
}
