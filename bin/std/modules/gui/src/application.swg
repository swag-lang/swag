#global public

struct Application
{
    renderer: Pixel.RenderOgl
}

impl Application
{
    mtd getRenderer() => &renderer

    mtd createSurface(title: string, x, y, width, height: s32, flags: SurfaceFlags)->*Surface throw
    {
        return try Surface.create(self, title, x, y, width, height, flags)
    }
}