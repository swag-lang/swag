#global testerror "Err0091"
#global testerror "Err0263"

const TOTO = false
#if TOTO
{
    #if TOTO
    {
        func toto()
        {
            @assert(false)
        }
    }
}

#if TOTO
{
    func toto() {}
}

#test
{
    toto()
}
