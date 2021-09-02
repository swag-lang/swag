#[Swag.EnumFlags]
enum Days
{
    None = 0b_0000_0000 // 0
	None1 = 0b_0000_0000
}

var g = @typeof(Days).name
var g1 = @typeof(Days).kind

#test
{
	@assert(g == "__compiler1099.Days")
	@assert(g1 == Swag.TypeInfoKind.Enum)
}