struct Err {}

func titi()->s32 throw { throw Err{}; return 0; }

#[Swag.Inline]
func toto()->s32 throw => titi()

#[Swag.Optimize(.All, false)]
#test
{
    discard catch toto()
    @assert(@err == Err)
}
