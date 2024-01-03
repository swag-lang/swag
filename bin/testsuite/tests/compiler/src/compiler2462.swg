#global testerror "San0009"
#global testerror "Saf0009"

#[Swag.Safety("overflow", true)]
#test
{
    var x: s32 = Swag.S32.Max
    x += 1
}