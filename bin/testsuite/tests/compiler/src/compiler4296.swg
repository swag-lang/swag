#global testerror "Err0705"

#test
{
    var i = 0
    #scope Scope
    {
        if i == 2:
            return

        while i < 5
        {
            i += 1
            if i == 2:
                continue Scope
            @assert(i < 2)
        }
    }
}