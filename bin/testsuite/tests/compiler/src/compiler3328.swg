#global testerror "Err0616"
#test
{
    var a = #run {
        loop 1 do
            return
        return true
    }

    @assert(a == 66.0)
}