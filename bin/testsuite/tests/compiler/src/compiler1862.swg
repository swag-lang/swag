impl enum RGB
{
    func isRed(me)->bool
    {
        return me == R
    }

    func isBlue(me)->bool => me == B
}

#test
{
    @assert(RGB.isRed(RGB.R))
    @assert(!RGB.isRed(RGB.B))
    @assert(RGB.isBlue(RGB.B))
}

enum RGB
{
    R
    G
    B
}
