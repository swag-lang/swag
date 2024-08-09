#global testerror "Err0578"
struct Point {}
impl Point
{
	func opEqual(self, other: Point) -> bool
	{
		return self.x == other.x and self.y == other.y
	}
}