#global testerror "Err0317"
func toto() {}
func titi() throw
{
    throw toto()
}