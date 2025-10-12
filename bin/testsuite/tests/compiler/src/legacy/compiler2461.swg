#global testerror "Saf0027"
#global testerror "Saf0027"

#[Swag.Safety(.All, .Overflow, true)]
#test
{
    var x: s16 = Swag.S16.Max
    x += 1
}
