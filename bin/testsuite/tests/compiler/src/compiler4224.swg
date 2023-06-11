func titi() throw { throw "0"; }

#[Swag.Inline]
func toto() throw = titi()

#[Swag.Optim("", false)]
#test
{
    catch toto()
    @assert(@errmsg == "0")
}