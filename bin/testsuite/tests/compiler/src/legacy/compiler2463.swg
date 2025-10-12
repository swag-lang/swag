#global testerror "Saf0027"
#global testerror "Saf0027"

#[Swag.Safety(.All, "overflow", true)]
#test
{
    var x: s64 = Swag.S64.Max
    x += 1
}
