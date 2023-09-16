#global testerror "Err1089"
func and() => true
{

}

#test
{
    @assert(and() and and())
}