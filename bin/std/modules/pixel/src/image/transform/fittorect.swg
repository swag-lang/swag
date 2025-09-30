using Core

public impl Image
{
    // Fits the image to fill the target rectangle while maintaining aspect ratio.
    // The image is scaled to cover the entire target area, then cropped to exact dimensions.
    // Uses center cropping to preserve the most important part of the image.
    mtd fitToRect(targetRect: Math.Rectangle)
    {
        let targetAspect = targetRect.width / targetRect.height
        let imageAspect  = .getAspectRatio()

        // Calculate dimensions to fill target while maintaining aspect ratio
        var newWidth, newHeight: s32

        if imageAspect > targetAspect
        {
            // Image is wider than target - scale by height
            newHeight = cast(s32) targetRect.height
            newWidth  = cast(s32) (newHeight * imageAspect)
        }
        else
        {
            // Image is taller than target - scale by width
            newWidth  = cast(s32) targetRect.width
            newHeight = cast(s32) (newWidth / imageAspect)
        }

        .resize(newWidth, newHeight, .Bicubic)

        // Center crop to exact target dimensions
        let cropX = (.width - cast(s32) targetRect.width) / 2
        let cropY = (.height - cast(s32) targetRect.height) / 2

        var cropRect: Math.Rectangle
        cropRect.x      = cropX
        cropRect.y      = cropY
        cropRect.width  = cast(s32) targetRect.width
        cropRect.height = cast(s32) targetRect.height

        .crop(cropRect)
    }
}
