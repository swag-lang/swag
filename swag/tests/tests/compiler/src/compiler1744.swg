
#global testerror

//#[swag.printbc]
#test
{
    let array: [2, 2] s32 = [[1, 2], [3, 4]]

    visit const *it: array
    {
        it[0] = 0
    }
}