#global public
using Core, Core.Drawing

enum BrushType
{
    SolidColor
    Texture
    Hatch
}

struct Brush
{
    color:      Color = Argb.White
    type:       BrushType = SolidColor
    hatch:      HatchStyle = Dot
    texture:    Texture
    uvMode:     UVMode = Fill
    uvRect:     Math.Rectangle
}

impl Brush
{
    // Returns a solid color brush
    func createSolid(color: Color)->Brush
    {
        var result: retval
        result.color = color
        result.type = .SolidColor
        return result
    }

    // Returns a full texture brush
    func createTexture(texture: Texture)->Brush
    {
        var result: retval
        result.color = Argb.White
        result.type = BrushType.Texture
        result.texture = texture
        result.uvMode = .Fill
        return result
    }

    // Returns a tiled texture brush
    func createTiled(texture: Texture)->Brush
    {
        var result: retval
        result.color = Argb.White
        result.type = BrushType.Texture
        result.texture = texture
        result.uvMode = .Tile
        return result
    }
}