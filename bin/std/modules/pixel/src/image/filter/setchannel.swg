using Core

impl Image
{
    #[Swag.EnumFlags]
    public enum ChannelMode
    {
        Red
        Green
        Blue
        Alpha
        RGB   = Red | Green | Blue
        RGBA  = RGB | Alpha
    }

    public enum ChannelValueMode
    {
        Color
        MinRGB
        MaxRGB
        MeanRGB
        Alpha
    }

    // Change the alpha channel
    public mtd setAlpha(value: u8 = 0)
    {
        if .pf != .BGRA8 do
            return
        .setChannel(Image.ChannelMode.Alpha, Image.ChannelValueMode.Color, value)
    }

    // Change specified color channels
    public mtd setChannel(mode: ChannelMode, value: ChannelValueMode, color: u8 = 0)
    {
        if .pf != .BGR8 and .pf != .BGRA8 do
            return

        // Remove alpha change if it does not exists
        var rmode = mode
        if .pf == .BGR8
        {
            rmode &= ~.Alpha
            if rmode == 0 or value == .Alpha do
                return
        }

        var f = {rmode, value, color}
        .visitPixels(&f)
        {
            let params = cast(*#decltype(f)) userData
            let mode   = params.rmode

            var color: u8 = undefined
            #[Swag.Complete]
            switch params.value
            {
            case Color:
                color = params.color
            case MinRGB:
                color = Math.min(pix[0], pix[1], pix[2])
            case MaxRGB:
                color = Math.max(pix[0], pix[1], pix[2])
            case MeanRGB:
                color = cast(u8) ((pix[0] + #prom pix[1] + #prom pix[2]) / 3)
            case Alpha:
                color = pix[3]
            }

            if mode & .Blue do
                pix[0] = color
            if mode & .Green do
                pix[1] = color
            if mode & .Red do
                pix[2] = color
            if mode & .Alpha do
                pix[3] = color
        }
    }
}
