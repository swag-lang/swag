#global testerror "Err0577"
#test
{
    var a = #run {
        loop 1 do
            return 66.0
        return
    }

    @assert(a == 66.0)
}