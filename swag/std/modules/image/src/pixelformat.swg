#global public
using swag

// Desribe the layout format of one image pixel
enum PixelFormat
{
    BGR8
    BGRA8
}

impl enum PixelFormat
{
    // Returns the bpp (bits per pixel) of the given pixel format
    #[constexpr]
    func bpp(self)->u8
    {
        #[complete]
        switch self
        {
            case BGR8:  return 24
            case BGRA8: return 32
        }

        return 0
    }
}