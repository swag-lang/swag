using core, drawing

impl Image
{
    public enum FillGradientType
    {
        Horizontal
    }
/*
    public func fillGradient(using self, fillType: FillGradientType, color0: Color = Argb.White) throw
    {
        if pf != PixelFormat.BGR8 && pf != PixelFormat.BGRA8
            throw "unsupported pixel format"

        f := @{col0: color0}

        using FillGradientType
        switch(fillType)
        {
        case Horizontal:
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor0 := x / cast(f32) image.width
            }
        }
    }*/
}