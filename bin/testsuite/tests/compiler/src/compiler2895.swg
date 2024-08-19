#global testerror "Err0183"
func and() => true
{

}

#test
{
    @assert(and() and and())
}