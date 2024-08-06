//#global testerror "Err0575"
#[Swag.Overload]
func min(x, y: s16) => 0
#[Swag.Overload]
func min(x, y: s32) => 0

#test
{
    var x = min(1's8, 2's16)
}