#[swag.printbc]
#test
{
	x := [1, 2, 3]
    y := [10, 20, 30]
    @memcpy(&x, &y, @sizeof(x))

    @print(cast(s64)x[2])
}