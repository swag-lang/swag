#test
{
    func toto(cb: nn func||())
    {
        @assert(cb == null)
        if cb:
            cb()
    }

    toto(null)
}