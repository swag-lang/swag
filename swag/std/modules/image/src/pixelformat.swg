#global public
using swag

// Desribe the layout format of one image pixel
enum PixelFormat
{
    R8G8B8
    A8R8G8B8
    B8G8R8
}

impl enum PixelFormat
{
    // Returns the bpp (bits per pixel) of the given pixel format
    #[constexpr]
    func bpp(self)->u32
    {
        #[complete]
        switch self
        {
            case R8G8B8:    return 24
            case B8G8R8:    return 24
            case A8R8G8B8:  return 32
        }

        return 0
    }
}