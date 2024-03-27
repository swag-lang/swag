#global testerror "Err0712"
#global testerror "Err0730"

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
    func toto()
    {
    }
}

#test
{
    toto()
}
