#global testerror "Saf0008"
#[Swag.Safety("", true)]
#test
{
    var x = cast(s64) (-1's64 - Swag.S64.Max)
    x = @abs(x)
}
