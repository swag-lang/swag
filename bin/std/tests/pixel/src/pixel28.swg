using Core, Pixel

func cmp(a, b: Image)
{
    var ptra = a.pixels.buffer
    var ptrb = b.pixels.buffer
    var m = 0
    loop a.size / 3
    {
        var ca = Color.fromRgb(ptra[0], ptra[1], ptra[2])
        var cb = Color.fromRgb(ptrb[0], ptrb[1], ptrb[2])
        m = Math.max(cast(s32) ca.r - cast(s32) cb.r, m)
        m = Math.max(cast(s32) ca.g - cast(s32) cb.g, m)
        m = Math.max(cast(s32) ca.b - cast(s32) cb.b, m)
        ptra, ptrb += 3
    }

    @assert(m == 11)
}

#test
{
    {
        var imgA = assume Image.load(imagePath("testA.jpg"))
        assume imgA.save(tmpPath("testA.jpg"))
        var imgB = assume Image.load(tmpPath("testA.jpg"))
        cmp(imgA, imgB)
    }

    try File.delete(tmpPath("testA.jpg"))
}
