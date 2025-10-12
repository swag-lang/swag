//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    let x = 1's16
    if x == 2's16 do
        @print("a")
}
