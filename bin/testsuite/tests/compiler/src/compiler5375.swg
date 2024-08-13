#global testerror "Err0705"
#test
{
    var l, dx, dy: s32

    const O = [{nl: 1, nx: 1, ny: 0}, {0, 0, -1}, {1, -1, 0}, {0, 0, 1}]
    visit v: O
    {
        l += v.item; dx = v.item1; dy = v.item2
    }
}