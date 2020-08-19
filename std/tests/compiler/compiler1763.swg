#module "compiler1763"
#unittest error

#[swag.printbc]
#test
{
/*    x := 255'u8
    x += 1
    @print(cast(s64) x)*/
    x := [1, 2]
    y := 0
    z := x[-1]
}