#global testerror "Err0212"
func and() => true
{

}

#test
{
    @assert(and() and and())
}