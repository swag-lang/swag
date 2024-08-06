#global testerror "Err0259"
func and() => true
{

}

#test
{
    @assert(and() and and())
}