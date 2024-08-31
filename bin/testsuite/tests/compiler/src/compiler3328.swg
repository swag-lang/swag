#global testerror "Err0649"
#test
{
    var a = #run {
        for 1:
            return
        return true
    }

    @assert(a == 66.0)
}