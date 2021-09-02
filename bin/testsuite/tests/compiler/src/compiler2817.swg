enum A: string
{
    R = "toto"
    G = "titi"
}

#test
{
    @assert(@stringof(A.R) == "A.R")
    @assert(@stringof(A.G) == "A.G")
}