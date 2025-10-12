/*var G: s32
var G1: s32

func toto()->const &s32
{
    return &G
}

func toto1()->const &s32
{
    return &G1
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    @assert(toto() == toto1())
    @assert(ref toto() != #ref toto1())
    @assert(ref toto() == cast(const &s32) &G)
    @assert(ref toto1() == cast(const &s32) &G1)
}
*/
