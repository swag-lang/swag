#module compiler1400
#unittest error

public struct A
{
    readonly x: s32
}
 
func titi()
{
    var pt: A
    pt.x = 0
}