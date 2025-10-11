//#[Swag.PrintBc]
#test
{
    func toto(cb: func||() = null)
    {
        @assert(cb == null)
        if cb:
            cb()
    }

    toto()
}
