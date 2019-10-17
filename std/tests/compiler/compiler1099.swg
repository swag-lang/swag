#[swag.flags]
enum Days
{
    None = 0b_0000_0000 // 0
	None1 = 0b_0000_0000
}

var g = @typeof(Days).name
var g1 = @typeof(Days).kind

#test
{
	@assert(g == "Days")
	@assert(g1 == swag.TypeinfoKind.Enum)
}