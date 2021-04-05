using core

impl Image
{
    public enum TurnAngle
    {
        T90
        T180
        T270
    }

    #[swag.safety("", false)]
    private func doTurn(using self, dstImage: *Image, angle: TurnAngle)
    {
        #[swag.mixin]
        func(BPP: u8) doIt()
        {
            f := self
            switch angle
            {
            case T90:
                dstImage.visitPixels(f)
                {
                    srcImage := cast(*Image) userData
                    nx := srcImage.width - y - 1
                    ny := x
                    pixSrc := srcImage.pixels + (nx * srcImage.bpp8) + (ny * srcImage.width8)
                    copyPixel'BPP(pix, pixSrc)
                }
                break

            case T180:
                dstImage.visitPixels(f)
                {
                    srcImage := cast(*Image) userData
                    nx := srcImage.width - x - 1
                    ny := srcImage.height - y - 1
                    pixSrc := srcImage.pixels + (nx * srcImage.bpp8) + (ny * srcImage.width8)
                    copyPixel'BPP(pix, pixSrc)
                }
                break

            case T270:
                dstImage.visitPixels(f)
                {
                    srcImage := cast(*Image) userData
                    nx := y
                    ny := srcImage.height - x - 1
                    pixSrc := srcImage.pixels + (nx * srcImage.bpp8) + (ny * srcImage.width8)
                    copyPixel'BPP(pix, pixSrc)
                }
                break
            }
        }

        switch bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: debug.assert(false)
        }
    }

    // Turn image by a given predefined angle
    public func turn(using self, angle: TurnAngle)
    {
        debug.assert(pixels != null)
        if angle == .T180
            allocWorkingBuffer()
        else
            allocWorkingBuffer(height, width, pf)
        doTurn(workingBuffer, angle)
        applyWorkingBuffer()
    }

    // Turn image by a given predefined angle
    public func toTurn(using self, angle: TurnAngle)->Image
    {
        debug.assert(pixels != null)

        var result: retval
        if angle == .T180
            result = create(width, height, pf)
        else
            result = create(height, width, pf)
        doTurn(&result, angle)
        return result
    }
}