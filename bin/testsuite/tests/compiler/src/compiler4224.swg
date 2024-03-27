struct Err{}

func titi() throw { throw Err{}; }

#[Swag.Inline]
func toto() throw = titi()

#[Swag.Optim("", false)]
#test
{
    catch toto()
    @assert(@err() == Err)
}