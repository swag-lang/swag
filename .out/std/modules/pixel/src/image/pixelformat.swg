#global public
using Swag

// Desribe the layout format of one image pixel
enum PixelFormat: u8
{
    BGR8
    BGRA8
    RGB8
    RGBA8
}

impl enum PixelFormat
{
    // Returns the bpp (bits per pixel) of the given pixel format
    #[ConstExpr]
    func bpp(self)->u8
    {
        #[complete]
        switch self
        {
            case BGR8:  return 24
            case BGRA8: return 32
            case RGB8:  return 24
            case RGBA8: return 32
        }

        return 0
    }

    // Returns the number of image channels
    #[ConstExpr]
    func channels(self)->u8
    {
        #[complete]
        switch self
        {
            case BGR8:  return 3
            case BGRA8: return 4
            case RGB8:  return 3
            case RGBA8: return 4
        }

        return 0
    }

    // Returns the bit depth of each channel
    #[ConstExpr]
    func bitDepth(self)->u8
    {
        #[complete]
        switch self
        {
            case BGR8:  return 8
            case BGRA8: return 8
            case RGB8:  return 8
            case RGBA8: return 8
        }

        return 0
    }

    // Returns true if the given pixel format has an alpha channel
    #[ConstExpr]
    func hasAlpha(self)->bool
    {
        #[complete]
        switch self
        {
            case BGR8:  return false
            case BGRA8: return true
            case RGB8:  return false
            case RGBA8: return true
        }

        return false
    }
}