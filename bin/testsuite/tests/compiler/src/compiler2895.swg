#global testerror "Err0289"
func and() => true
{

}

#test
{
    @assert(and() and and())
}