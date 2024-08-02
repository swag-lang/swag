#global testerror "Err0374"
func toto() {}
func titi() throw
{
    throw toto()
}