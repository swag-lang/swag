//#[swag.printbc]
func toto(x: s64, z: const [4] s64)
{
    cpt := 0
    while cpt < x
    {
        @print(z[cpt])
        cpt += 1
    }
}

#[swag.printbc]
#test
{
    z := "toto"
    x := [10's64, 20, 30, 40]
    toto(4, x)
}