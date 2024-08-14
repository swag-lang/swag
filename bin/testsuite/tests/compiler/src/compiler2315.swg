
#global testerror "Err0583"

#[Swag.Strict]
typealias Alias = s32

func toto(x: Alias)
{
}

#test
{
    toto(0)
}