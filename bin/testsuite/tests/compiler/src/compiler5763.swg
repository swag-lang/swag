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
    A.toto(null)
}