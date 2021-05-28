#global public
using Core, Drawing, Core.Math

impl Painter
{
    func drawImage(using self, texture: Texture, left, top, width, height: f32)
    {
        cmd := newCommand(CommandId.BindImage, true)
        cmd.params.image.handle = texture.handle

        fillRect(left, top, width, height, Argb.White)

        cmd = newCommand(CommandId.BindImage, true)
        cmd.params.image.handle = null
    }
}