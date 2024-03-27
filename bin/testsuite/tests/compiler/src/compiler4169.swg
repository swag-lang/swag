#global testerror "San0008"

#[Swag.Optim("", false), Swag.Safety("sanity", true)]
func toto(param: s32)
{
    var pix:   [4] s32
    var color: u8 = undefined

    //#[Swag.Complete]
    switch param
    {
    case 0:
        color = 0
    case 1:
        color = 1
    }

    if param == 0 do
        pix[0] = color
    if param == 1 do
        pix[1] = color
}
