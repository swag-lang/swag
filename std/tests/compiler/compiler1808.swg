#[swag.printbc]
#test
{
    a := @[1's64, 2, 4]
    c := a[1..2]
    @print(c[0])
    @print(" ")
    @print(cast(s64) @countof(c))
}