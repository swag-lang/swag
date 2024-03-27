//#[Swag.PrintBc]
#test
{
    func toto(cb: closure() = null)
    {
        @assert(cb == null)
        if cb do
            cb()
    }

    toto()
}
