#global testerror "Err0288"
#test
{
    var a = #run
    {
        for 1 do
            return 66.0
        return true
    }

    @assert(a == 66.0)
}
