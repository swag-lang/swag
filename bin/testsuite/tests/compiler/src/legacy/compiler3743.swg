struct Err {}

func titi()->s32 throw { throw Err{}; return 0; }
func toto() throw => titi()

//#[Swag.PrintBc]
#test
{
    discard catch toto()
    @assert(@err == Err)
}
