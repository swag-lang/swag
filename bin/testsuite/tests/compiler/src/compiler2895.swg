#global testerror "Err0284"
func and() => true
{

}

#test
{
    @assert(and() and and())
}