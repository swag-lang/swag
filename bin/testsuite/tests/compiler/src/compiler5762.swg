//#global skip
#global testerror "San0001"

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

#[Swag.Sanity(true)]
#[Swag.Safety("null", true)]
#[Swag.Optimize("", false)]
#test
{
    var p: *A
    let p1 = p
    A.toto(p1, 0)
}