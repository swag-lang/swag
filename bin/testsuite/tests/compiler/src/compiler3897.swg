//#global testerror "Err0506"
#[Swag.Overload]
{
func min(x, y: s8) => 0
func min(x, y: s16) => 0
func min(x, y: s32) => 0
}

#test
{
    var x = min(1's32, 2's16)
}