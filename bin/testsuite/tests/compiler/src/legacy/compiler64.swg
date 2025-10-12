#global testerror "Saf0027"

#[Swag.Safety(.All, .All, true)]
#test
{
    var a: s8 = 127's8 + 1's8
}
