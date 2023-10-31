func titi()->s32 throw { throw "0"; return 0; }
func toto() throw => titi()

#test
{
    discard catch toto()
    @assert(cast(string) @err() == "0")
}