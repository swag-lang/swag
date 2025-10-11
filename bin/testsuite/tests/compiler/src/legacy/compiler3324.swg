//#global skip
//#global testerror
#test
{
    let a = #run
    {
        var ret = 0's32
        for 10 do
            ret += 66
        return ret
    }

    @assert(a == 660)
}
