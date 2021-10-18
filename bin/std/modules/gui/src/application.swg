#global public

struct Application
{
    renderer: Pixel.RenderOgl
}

impl Application
{
    mtd getRenderer() => &renderer

    // Creates a new surface
    mtd createSurface(title: string, x, y, width, height: s32, flags: SurfaceFlags, onEvent: func(*Wnd, *Event) = null)->*Surface throw
    {
        return try Surface.create(self, title, x, y, width, height, flags, onEvent)
    }
}