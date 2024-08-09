#global testerror "Err0256"
func and() => true
{

}

#test
{
    @assert(and() and and())
}