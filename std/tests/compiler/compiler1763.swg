#[swag.printbc]
#test
{
    x := true
    x = !x
    @print(cast(s64) x)
    x = !x
    @print(cast(s64) x)
}