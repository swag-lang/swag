#global testerror "Saf0008"
#[Swag.Safety(.All, "", true)]
#test
{
    var x = cast(s16) (-1's16 - Swag.S16.Max)
    x = @abs(x)
}
