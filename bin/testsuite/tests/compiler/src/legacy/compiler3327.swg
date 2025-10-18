#global testerror "Err0185"
#test
{
    var a = #run
    {
        for 1 do
            return 66.0
        return
    }

    @assert(a == 66.0)
}
