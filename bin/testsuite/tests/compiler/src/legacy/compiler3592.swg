#test
{
    func toto(cb: func||())
    {
        @assert(cb == null)
        if cb:
            cb()
    }

    toto(null)
}
