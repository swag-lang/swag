#global testerror "Err0394"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
    var ptr = toto
}
