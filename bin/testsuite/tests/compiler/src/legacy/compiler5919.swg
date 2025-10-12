//#global skip
//#global testerror "Saf0030"

var G = 0

struct A {}
impl A
{
    #[Swag.Optimize("", false)]
    mtd toto()
    {
        //@print("titi")
        G = 1
    }
}

#[Swag.Sanity(true), Swag.Safety("", "null", false), Swag.Optimize("", false)]
#test
{
    A.toto(null)
}
