#global testerror "Err0382"
func toto() {}
func titi() throw
{
    throw toto()
}