var G = 0
func toto(x: s32)
{
    let v = closure|x|() {
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