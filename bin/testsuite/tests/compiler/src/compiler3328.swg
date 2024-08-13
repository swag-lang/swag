#global testerror "Err0626"
#test
{
    var a = #run {
        loop 1 do
            return
        return true
    }

    @assert(a == 66.0)
}