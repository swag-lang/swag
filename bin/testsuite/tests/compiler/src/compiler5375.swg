#global testerror "Err0684"
#test
{
    var l, dx, dy: s32

    const O = [{nl1: 1, nx: 1, ny: 0}, {0, 0, -1}, {1, -1, 0}, {0, 0, 1}]
    foreach v in O
    {
        l += v.item; dx = v.item1; dy = v.item2
    }
}