#test
{
    func toto(cb: closure())
    {
        @assert(cb == null)
        if cb do
            cb()
    }

    toto(null)
}