//#global skip
#global testerror
#test
{
    var a = #run {
        loop 1
            return
        return true
    }

    @assert(a == 66.0)
}