#[swag.printbc]
func toto(x: s64, y: string, z: const *s64)
{
    @print(x)
    @print(y)
    @print(z[0])
    @print(z[1])
    @print(z[2])
    @print(z[3])
}

#test
{
    x := [10's64, 20, 30, 40]
    toto(10, "myString", &x[0])
}