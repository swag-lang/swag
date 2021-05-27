#global public
using Core, Drawing, Core.Math

impl Painter
{
    func drawImage(using self, image: ImageHandle, x, y: f32)
    {
        cmd := newCommand(CommandId.BindImage, true)
        cmd.params.image.handle = image
        fillRect(x, y, 100, 100, Argb.Red)
    }
}