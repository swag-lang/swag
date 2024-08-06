#global testerror "Err0579"
#test
{
    var a = #run {
        loop 1 do
            return
        return true
    }

    @assert(a == 66.0)
}