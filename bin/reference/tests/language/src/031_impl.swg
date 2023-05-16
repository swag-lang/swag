/**
`impl` can be used to declare some stuff in the scope of an enum. The keyword `self` represents the enum value.
*/

enum RGB { R; G; B; }

/**
Note the `impl enum` syntax. We'll see later that `impl` is also used for structs.
*/
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

    // A first look at 'ufcs' (uniform function call syntax)
    @assert(R.isRedOrBlue())
    @assert(!RGB.G.isRedOrBlue())
}