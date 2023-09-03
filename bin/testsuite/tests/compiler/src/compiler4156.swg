#global testerror "Err0469"

//#[Swag.PrintBc]
#[Swag.Safety("sanity", true)]
func toto()->*s32
{
    let a = 0
    return &a
}