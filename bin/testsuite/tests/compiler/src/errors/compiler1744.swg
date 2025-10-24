//#global testerror

//#[Swag.PrintBc]
#test
{
    var array: [2, 2] s32 = [[1, 2], [3, 4]]

    foreach &it in array
    {
        it[0] = 0
    }
}
