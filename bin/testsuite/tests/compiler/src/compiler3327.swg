#global testerror "Err0473"
#test
{
    var a = #run {
        for 1:
            return 66.0
        return
    }

    @assert(a == 66.0)
}