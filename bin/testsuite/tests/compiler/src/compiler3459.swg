var G = 0
func toto(x: s32)
{
    v := @|x|() {
        G += x
    }

    v()
}

#test
{
    toto(10)
    toto(20)
    @assert(G == 30)
}