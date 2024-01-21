#global testerror "Err0295"
func and() => true
{

}

#test
{
    @assert(and() and and())
}