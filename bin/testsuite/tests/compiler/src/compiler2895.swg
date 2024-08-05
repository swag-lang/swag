#global testerror "Err0290"
func and() => true
{

}

#test
{
    @assert(and() and and())
}