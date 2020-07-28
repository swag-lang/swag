#[swag.printbc]
func toto(x: s64, y: string, z: const [4] s64)
{
    @print(x)
    @print(y)
    @print(z[0])
}

#test
{
    toto(10, "myString", [10, 20, 30, 40])
}