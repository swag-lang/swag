#global testerror "Err0296"
func and() => true
{

}

#test
{
    @assert(and() and and())
}