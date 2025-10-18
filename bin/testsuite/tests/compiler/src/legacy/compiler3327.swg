#global testerror "Err0497"
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
