//#global skip
//#global testerror
#test
{
    a := #run {
        ret := 0's32
        loop 10 ret += 66
        return ret
    }

    @assert(a == 660)
}