#[swag.printbc]
#test
{
	x := [1, 2, 3]
    y := [10, 20, 30]
    @memset(&x, 0, @sizeof(x))

    @print(cast(s64)x[2])
}