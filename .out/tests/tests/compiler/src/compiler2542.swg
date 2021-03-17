using swag

#[inline]
func toto()->s32 throw
{
    throw "coucou"
    return 10
}

//#[printbc, optim("", false)]
func titi() throw
{
    x := try toto()
    G = 1
}

var G: u32

#test
{
    catch titi()
    @assert(G == 0)
}