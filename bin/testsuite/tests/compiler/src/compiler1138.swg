#[Swag.Inline]
func toto()->s32 => 1

//#[Swag.PrintBc]
#test
{
    var r = toto()
}
