#[Swag.EnumFlags]
enum A: u32
{
    R
    G
}

#test
{
    @assert(@stringof(A.G) == "A.G")
    @assert(@stringof(A.R | A.G) == "A.R|A.G")
}