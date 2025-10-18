#global testerror "Err0629"

func toto()->s32 => 666

#[Swag.PrintBc]
#test
{
    var ptr = toto
}
