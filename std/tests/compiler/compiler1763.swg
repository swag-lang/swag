var x: [2] s32 = [10, 2]

#[swag.printbc]
#test
{
    @print(cast(s64)x[0])
}