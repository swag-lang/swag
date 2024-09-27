//#[Swag.PrintBc]
#test
{
    func toto(cb: nn func||() = null)
    {
        @assert(cb == null)
        if cb:
            cb()
    }

    toto()
}