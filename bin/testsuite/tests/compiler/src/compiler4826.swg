#global testerror "San0010"
#global testerror "Saf0027"
#[Swag.Safety("overflow", true)]
#[Swag.Optimize("", false)]
#test
{
    var x: s32 = Swag.S32.Max
    x = x + 2
}
