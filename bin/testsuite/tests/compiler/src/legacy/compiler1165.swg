#global testerror "Err0090"
#global testerror "Err0213"

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
