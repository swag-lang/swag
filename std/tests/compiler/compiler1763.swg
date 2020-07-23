func toto(x: [2] s32)
{
    @print(cast(s64) x[1])
}

#[swag.printbc]
#test
{
    toto([1, 2])
}