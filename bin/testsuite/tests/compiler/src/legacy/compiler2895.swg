#global testerror "Err0430"
func and() => true
{

}

#test
{
    @assert(and() and and())
}