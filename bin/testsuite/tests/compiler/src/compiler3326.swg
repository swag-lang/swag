#global testerror "Err0625"
#test
{
    var a = #run {
        loop 1 do
            return 66.0
        return true
    }

    @assert(a == 66.0)
}