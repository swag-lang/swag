#global testerror "Saf0027"
#global testerror "Saf0027"

#[Swag.Safety(.All, "overflow", true)]
#test
{
    var x: s8 = 0
    x += 100
    x += 100
}
