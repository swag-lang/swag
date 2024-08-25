#global testerror "San0010"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#[Swag.Safety("sanity", true)]
#[Swag.Safety("overflow", true)]
#test
{
    var x = 1'u8
    let y = 1'u8
    x -= y
    x -= y
}