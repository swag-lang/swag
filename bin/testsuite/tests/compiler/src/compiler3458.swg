var G = 0
func toto()
{
    G = 10
}

#test
{
    v := @|&toto|() {
        toto()
    }

    v()
    @assert(G == 10)
}