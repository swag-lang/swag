#global testerror "San0001"

struct A {}
impl A
{
    mtd toto()
    {
        @print("titi")
    }
}

#[Swag.Safety("sanity", true)]
#test
{
    var p: *A
    A.toto(p)
}