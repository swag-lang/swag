#global testerror "Err0603"
#test
{
    var a = #run
    {
        for 1 do
            return
        return true
    }

    @assert(a == 66.0)
}
