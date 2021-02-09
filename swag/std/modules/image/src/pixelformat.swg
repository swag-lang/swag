#global public
using swag

// Desribe the layout format of one image pixel
enum PixelFormat
{
    A8R8G8B8
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
            case A8R8G8B8:
                return 32
        }

        return 0
    }
}