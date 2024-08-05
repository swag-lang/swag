//#global testerror "Err0715"

#test
{
    var l, dx, dy: s32

    const O = [{nl: 1, nx: 1, ny: 0}, {0, 0, -1}, {1, -1, 0}, {0, 0, 1}]
    visit v: O
    {
        l += v.item0; dx = v.item1; dy = v.item2
    }
}