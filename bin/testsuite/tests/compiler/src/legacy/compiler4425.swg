using Swag

struct A {}
impl A
{
    #[Implicit]
    mtd opAffect(x: s32) {}
}

var G = 0
#[Swag.Overload]
func toto(x: &&A)
{
    G = 1
}

#[Swag.Overload]
func toto(x: A)
{
    @assert(false)
}

//#[Swag.PrintBc]
#test
{
    toto(0's32)
    @assert(G == 1)
}
