#[swag.printbc]
#test
{
	x := [1, 2, 3]
    y := [10, 20, 30]
    z := @memcmp(&x, &x, @sizeof(x))
    @print(cast(s64)z)
}