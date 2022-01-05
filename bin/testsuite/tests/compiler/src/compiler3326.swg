//#global skip
//#global testerror
#test
{
    a := #run {
        ret := 0's32
        loop 10
        {
            ret += 66
            if ret == 66
                return 66.0
        }

        return ret
    }

    @assert(a == 66.0)
}