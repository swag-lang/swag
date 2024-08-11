#global testerror "Err0175"
func and() => true
{

}

#test
{
    @assert(and() and and())
}