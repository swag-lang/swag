#global testerror "Err0474"
func toto() {}
func titi() throw
{
    throw toto()
}
