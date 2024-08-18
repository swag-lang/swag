#test
{
    func toto(cb: closure())
    {
        @assert(cb == null)
        if cb:
            cb()
    }

    toto(null)
}