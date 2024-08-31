#global testerror "Err0213"
func and() => true
{

}

#test
{
    @assert(and() and and())
}