#global testerror "Err0665"
func and() => true
{

}

#test
{
    @assert(and() and and())
}