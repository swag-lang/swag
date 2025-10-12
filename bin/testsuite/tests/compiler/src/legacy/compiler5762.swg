//#global skip
#global testerror "Saf0030"

var G = 0
struct A {}
impl A
{
    #[Swag.Optimize("", false)]
    mtd toto(x: s32)
    {
        //@print("titi")
        G = 1
    }
}

#[Swag.Sanity(true), Swag.Safety("null", true), Swag.Optimize("", false)]
func test()
{
    var p: *A
    let p1 = p
    A.toto(p1, 0)
}
