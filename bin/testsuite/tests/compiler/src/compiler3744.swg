func titi() throw { throw "0"; }
func toto() throw = titi()

#test
{
    catch toto()
    @assert(@err == "0")
}