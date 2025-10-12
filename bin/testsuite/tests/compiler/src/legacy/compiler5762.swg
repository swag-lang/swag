//#global skip
#global testerror "Saf0030"

var G = 0
struct A {}
impl A
{
    #[Swag.Optimize(.All, false)]
    mtd toto(x: s32)
    {
        //@print("titi")
        G = 1
    }
}

#[Swag.Sanity(true), Swag.Safety(.All, .Null, true), Swag.Optimize(.All, false)]
func test()
{
    var p: *A
    let p1 = p
    A.toto(p1, 0)
}
