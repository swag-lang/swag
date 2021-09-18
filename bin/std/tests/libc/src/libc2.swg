using Libc

#test
{
    x := atof(@dataof("5.5"))
    @assert(x >= 5.4 and x <= 5.6)
}