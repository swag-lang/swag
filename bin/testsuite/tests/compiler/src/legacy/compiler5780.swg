#global skip
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

#[Swag.NoInline]
func titi()->#null *A
{
    return null
}

#[Swag.Sanity(true)]
#[Swag.Safety(.All, .Null, true)]
#[Swag.Optimize(.All, false)]
//#[Swag.PrintGenBc]
func test()
{
    let p1 = titi()
    A.toto(p1, 0)
}