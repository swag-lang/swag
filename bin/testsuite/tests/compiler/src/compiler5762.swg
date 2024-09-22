#global testerror "San0001"

struct A {}
impl A
{
    mtd toto(x: s32)
    {
        @print("titi")
    }
}

#[Swag.Safety("sanity", true)]
#test
{
    var p: *A
    A.toto(p, 0)
}