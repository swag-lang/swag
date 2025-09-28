#global testerror "Err0191"
func and() => true
{

}

#test
{
    @assert(and() and and())
}