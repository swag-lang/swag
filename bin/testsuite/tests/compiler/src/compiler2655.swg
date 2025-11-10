#[Swag.Overload]
func(T) min(x, y: T) => x < y ? x : y
#[Swag.Overload]
func(T) min(x, y, z: T) => min(min(x, y), z)

#test
{
    var x = min(1's32, 2's32)
}
