var g_RedImg: s32
func addAliens()
{
    const DIST = 50 // DIST should be a numeric value, not a string
    for xc in 10
    {
        for yc in 6
        {
            let x = (xc * DIST) + 40
            let y = 140 + yc * DIST

            var type = &g_RedImg
            if yc == 0 do
                type = &g_RedImg
            elif yc >= 1 and yc <= 2 do
                type = &g_RedImg
            //g_Aliens.add({x, y, type})
        }
    }
}
