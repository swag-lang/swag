#global testerror "Err0234"
func and() => true
{

}

#test
{
    @assert(and() and and())
}