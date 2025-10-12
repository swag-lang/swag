#global testerror "Saf0004"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
func arithmeticMean(numbers: ...)
{
    #[Swag.Safety(.All, .All, true)]
    var cpt = cast(s32) numbers[1]
}

//#[Swag.PrintBc]
#test
{
    arithmeticMean(10)
}
