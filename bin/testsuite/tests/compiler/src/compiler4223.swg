func titi()->s32 throw { throw "0"; return 0; }

#[Swag.Inline]
func toto()->s32 throw => titi()

#[Swag.Optim("", false)]
#test
{
    discard catch toto()
    @assert(@errmsg == "0")
}