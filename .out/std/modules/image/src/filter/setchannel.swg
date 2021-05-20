using core

impl Image
{
    #[Swag.enumflags]
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
        if pf != .BGR8 and pf != .BGRA8 return // unsupported

        // Remove alpha change if it does not exists
        rmode := mode
        if pf == .BGR8
        {
            rmode &= ~.Alpha
            if rmode == 0 or value == .Alpha return
        }

        f := @{rmode, value, color}
        visitPixels(&f)
        {
            params := cast(*@typeof(f)) userData
            mode := params.rmode

            var color: u8 = ?
            #[Swag.complete]
            switch params.value
            {
            case Color:     color = params.color
            case MinRGB:    color = Math.min(pix[0], pix[1], pix[2])
            case MaxRGB:    color = Math.max(pix[0], pix[1], pix[2])
            case MeanRGB:   color = cast(u8) ((pix[0] + pix[1] + pix[2]) / 3)
            case Alpha:     color = pix[3]
            }

            if mode & .Blue  pix[0] = color
            if mode & .Green pix[1] = color
            if mode & .Red   pix[2] = color
            if mode & .Alpha pix[3] = color
        }
    }
}