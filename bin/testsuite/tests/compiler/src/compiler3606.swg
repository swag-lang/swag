alias MyType = s32

enum E: MyType
{
    R;
}

#test
{
    var arr: [2] s32
    arr[cast(uint) E.R] = 0
}