#global testerror "Syn0089"
func and() => true
{

}

#test
{
    @assert(and() and and())
}