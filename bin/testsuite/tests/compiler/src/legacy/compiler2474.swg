#global testerror "Saf0027"
#global testerror "Saf0027"
#[Swag.Safety("overflow", true), Swag.Optimize("", false)]
#test
{
    var x: u32 = Swag.U32.Max
    x = x * 2
}
