#global testerror "Saf0034"

#[Swag.Optimize("", false)]
#[Swag.Sanity(true)]
func toto(param: s32)
{
    var pix: [4] s32
    var color: u8 = undefined

    #[Swag.Complete]
    switch param
    {
    case 0: pix[0] = color
    case 1: color = 1
    }

    if param == 0: pix[0] = color
    if param == 1: pix[1] = color
}