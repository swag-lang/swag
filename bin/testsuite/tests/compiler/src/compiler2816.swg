enum A { R; G; B; }
#test
{
    @assert(@stringof(A.R) == "A.R")
    @assert(@stringof(A.G) == "A.G")
    @assert(@stringof(A.B) == "A.B")
    @assert(@stringof(0) == "0")
    @assert(@stringof(1+4) == "5")
    @assert(@stringof(A) == "A")
    //@print(@stringof(A))
}