var G = 0
func toto()
{
    G = 1
}

namealias O = toto
namealias P = O

#test
{
    P()
    @assert(G == 1)
}
