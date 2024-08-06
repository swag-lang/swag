
#global testerror "Err0647"

#[Swag.Strict]
typealias Alias = s32

func toto(x: Alias)
{
}

#test
{
    toto(0)
}