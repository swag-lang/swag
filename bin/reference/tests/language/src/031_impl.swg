// 'impl' can be used to declare some stuff in the scope of an enum

enum RGB { R; G; B; }
impl enum RGB
{
    func isRed(self)       => self == R
    func isRedOrBlue(self) => self == R or self == B
}

#test
{
    @assert(RGB.isRed(RGB.R))
    @assert(RGB.isRedOrBlue(RGB.B))

    using RGB
    @assert(isRedOrBlue(R))
    @assert(isRedOrBlue(B))
    @assert(R.isRedOrBlue())
    @assert(!RGB.G.isRedOrBlue())
}