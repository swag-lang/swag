#global testerror "Err0235"
func and() => true
{

}

#test
{
    @assert(and() and and())
}