
#global testerror "Err0609"

#[Swag.Strict]
typealias Alias = s32

func toto(x: Alias)
{
}

#test
{
    toto(0)
}