const TOTO = false
const TITI = true

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
#elif TITI and !TOTO
{
    func toto() {}
}

#test
{
    toto()
}
