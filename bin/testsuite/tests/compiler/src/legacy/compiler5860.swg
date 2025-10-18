#global testerror "Err0551"

#test
{
    @assert(#typeof(@args()) == const [..] string)
}