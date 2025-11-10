//#global skip
const CharProperties: [4] bool = [false, false, false, false]

func isDigit(c: u8) => CharProperties[c]

//#[Swag.PrintGenBc]
func compareLeft(src, dst: const [..] u8)->s32
{
    var csrc = @countof(src)
    var cdst = @countof(dst)
    var ai, bi = 0
    for
    {
        let ca = !csrc ? 0 : src[0]
        let cb = !cdst ? 0 : dst[0]

        let dga = isDigit(ca)
        let dgb = isDigit(cb)

        if !dga and !dgb do
            return 0
        if !dga do
            return -1
        if !dgb do
            return 1

        //ai, bi += 1
        if csrc do csrc -= 1
        if cdst do cdst -= 1
    }

    return 0
}
