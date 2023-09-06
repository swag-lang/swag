using Core

impl Image
{
    public enum TurnAngle
    {
        T90
        T180
        T270
    }

    #[Swag.Safety("", false)]
    mtd const doTurn(dstImage: *Image, angle: TurnAngle)
    {
        #[Swag.Mixin]
        func(BPP: u8) doIt()
        {
            var f = {img: self}
            switch angle
            {
            case T90:
                dstImage.visitPixels(&f)
                {
                    let data = cast(*@decltype(f)) userData
                    let srcImage = cast(const *Image) data.img
                    let nx = srcImage.width - y - 1
                    let ny = x
                    let pixSrc = srcImage.pixels.buffer + (nx * srcImage.bpp8) + (ny * srcImage.width8)
                    copyPixel'BPP(pix, pixSrc)
                }
                break

            case T180:
                dstImage.visitPixels(&f)
                {
                    let data = cast(*@decltype(f)) userData
                    let srcImage = cast(const *Image) data.img
                    let nx = srcImage.width - x - 1
                    let ny = srcImage.height - y - 1
                    let pixSrc = srcImage.pixels.buffer + (nx * srcImage.bpp8) + (ny * srcImage.width8)
                    copyPixel'BPP(pix, pixSrc)
                }
                break

            case T270:
                dstImage.visitPixels(&f)
                {
                    let data = cast(*@decltype(f)) userData
                    let srcImage = cast(const *Image) data.img
                    let nx = y
                    let ny = srcImage.height - x - 1
                    let pixSrc = srcImage.pixels.buffer + (nx * srcImage.bpp8) + (ny * srcImage.width8)
                    copyPixel'BPP(pix, pixSrc)
                }
                break
            }
        }

        switch bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: Debug.assert(false)
        }
    }

    // Turn image by a given predefined angle
    public mtd turn(angle: TurnAngle)
    {
        Debug.assert(pixels.buffer != null)
        if angle == .T180
            allocWorkingBuffer()
        else
            allocWorkingBuffer(height, width, pf)
        doTurn(workingBuffer, angle)
        applyWorkingBuffer()
    }

    // Turn image by a given predefined angle
    public mtd const toTurn(angle: TurnAngle)->Image
    {
        Debug.assert(pixels.buffer != null)

        var result: retval
        if angle == .T180
            result = create(width, height, pf)
        else
            result = create(height, width, pf)
        doTurn(&result, angle)
        return result
    }
}