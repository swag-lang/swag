//#[Swag.PrintBc]
#test
{
    func toto(cb: closure() = null)
    {
        @assert(cb == null)
        if cb:
            cb()
    }

    toto()
}