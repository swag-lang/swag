#global testerror "Err0528"

#test
{
    @assert(#typeof(@args()) == const [..] string)
}