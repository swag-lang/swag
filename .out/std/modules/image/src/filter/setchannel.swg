using core

impl Image
{
    #[swag.enumflags]
    public enum ChannelMode
    {
        Red
        Green
        Blue
        Alpha
        RGB  = Red | Green | Blue
        RGBA = RGB | Alpha
    }

    public enum ChannelValueMode
    {
        Color
        MinRGB
        MaxRGB
        MeanRGB
        Alpha
    }

    // Change specified color channels
    public func setChannel(using self, mode: ChannelMode, value: ChannelValueMode, color: u8 = 0)
    {
        debug.assert(pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8)

        // Remove alpha change if it does not exists
        rmode := mode
        if pf == PixelFormat.BGR8
        {
            rmode &= ~ChannelMode.Alpha
            if rmode == 0 || value == ChannelValueMode.Alpha return
        }

        f := @{rmode, value, color}
        visitPixels(&f)
        {
            params := cast(*@typeof(f)) userData
            mode := params.rmode

            var color: u8 = ?
            #[swag.complete]
            switch params.value
            {
            case Color:     color = params.color
            case MinRGB:    color = math.min(pix[0], pix[1], pix[2])
            case MaxRGB:    color = math.max(pix[0], pix[1], pix[2])
            case MeanRGB:   color = cast(u8) ((pix[0] + pix[1] + pix[2]) / 3)
            case Alpha:     color = pix[3]
            }

            if mode & ChannelMode.Blue  pix[0] = color
            if mode & ChannelMode.Green pix[1] = color
            if mode & ChannelMode.Red   pix[2] = color
            if mode & ChannelMode.Alpha pix[3] = color
        }
    }
}