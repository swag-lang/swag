#global testerror "Err0173"
func and() => true
{

}

#test
{
    @assert(and() and and())
}