#global testerror "Err0186"
func and() => true
{

}

#test
{
    @assert(and() and and())
}