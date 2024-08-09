#global testerror "Err0714"

#test
{
    var i = 0
    #scope Scope
    {
        if i == 2 do
            return

        while i < 5
        {
            i += 1
            if i == 2 do
                continue Scope
            @assert(i < 2)
        }
    }
}