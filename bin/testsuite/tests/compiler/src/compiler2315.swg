
#global testerror "Err0649"

#[Swag.Strict]
typealias T = s32

func toto(x: T)
{
}

#test
{
    toto(0)
}