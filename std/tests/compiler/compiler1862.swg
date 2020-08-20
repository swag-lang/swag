impl enum RGB
{
    func isRed(self)->bool
    {
        return self == R
    }

    func isBlue(self)->bool => self == B
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