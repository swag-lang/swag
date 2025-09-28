#global testerror "Err0786"

#test
{
    @assert(#typeof(@args()) == const [..] string)
}