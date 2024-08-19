#global testerror "Err0185"
func and() => true
{

}

#test
{
    @assert(and() and and())
}