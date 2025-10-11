#global testerror "Err0246"
func toto() {}
func titi() throw
{
    throw toto()
}
