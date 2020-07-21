var x: [2] s32 = [10, 20]

#[swag.printbc]
#test
{
    @print(cast(s64)x[1])
}