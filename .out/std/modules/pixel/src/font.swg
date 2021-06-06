#global public
using Core, FreeType

struct GlyphAtlas
{
    texture:    Texture
    width:      u32
    numGlyphs:  u32
    maxGlyphs:  u32
    datas:      Array'u8
    dirty:      bool
}

struct GlyphRef
{
    uv:             Math.Rectangle
    atlasIndex:     s32 = -1
    bearingLeft:    f32
    bearingRight:   f32
    ascent:         f32
    descent:        f32
}

struct Font
{
    typeFace:   const *TypeFace
    size:       u32
    atlases:    Array'GlyphAtlas
    asciiRef:   [128] GlyphRef
    dirty:      bool
}

impl Font
{
    // Creates a new Font of the given typeface and size
    func create(typeFace: TypeFace, size: u32)->Font
    {
        var result: retval
        result.typeFace = typeFace
        result.size = size
        return result
    }

    func getGlyphRef(using self, c: rune)->const *GlyphRef
    {
        // Already there ?
        if c < 128
        {
            if asciiRef[cast(u32) c].atlasIndex >= 0
                return &asciiRef[cast(u32) c]
        }
        else
        {
            Debug.assert(false)
        }

        // Render the glyph
        assume FT_Set_Pixel_Sizes(typeFace.face, 0, size)
        assume FT_Load_Char(typeFace.face, cast(FT_ULong) c, FT_LOAD_DEFAULT)
        assume FT_Render_Glyph(typeFace.face.glyph, FT_Render_Mode.FT_RENDER_MODE_NORMAL)

        // Creates a new atlas if this is the first time, or if the last atlas is full
        atlas := atlases.count ? atlases[atlases.count - 1] : null
        if atlases.isEmpty() or (atlas and atlas.numGlyphs == atlas.maxGlyphs * atlas.maxGlyphs)
        {
            var newAtlas: GlyphAtlas
            newAtlas.maxGlyphs = 8
            newAtlas.width = size * newAtlas.maxGlyphs
            newAtlas.datas.resize(cast(uint) newAtlas.width * newAtlas.width)
            Memory.clear(newAtlas.datas.buffer, newAtlas.datas.count)
            atlases.add(newAtlas)
        }

        // Copy the rendered glyph to the atlas datas
        atlas = atlases[atlases.count - 1]
        x := atlas.numGlyphs % atlas.maxGlyphs
        y := atlas.numGlyphs / atlas.maxGlyphs
        atlas.numGlyphs += 1

        buffer := atlas.datas.buffer + (x * size) + (atlas.width * (atlas.width - 1))
        buffer -= (y * atlas.width * size)
        bmp := typeFace.face.glyph.bitmap
        src := bmp.buffer
        loop j: bmp.rows
        {
            Memory.copy(buffer, src, cast(uint) bmp.width)
            src += bmp.pitch
            buffer -= atlas.width
        }

        // Setup GlyphRef
        var ref: *GlyphRef
        if c < 128
            ref = &asciiRef[cast(u32) c]
        else
            Debug.assert(false)

        ref.uv.x = (x * size) / cast(f32) atlas.width
        ref.uv.y = (y * size) / cast(f32) atlas.width
        ref.uv.width = size / cast(f32) atlas.width
        ref.uv.height = ref.uv.width

        ref.atlasIndex = cast(u32) (atlases.count - 1)

        metrics := &typeFace.face.glyph.metrics
        ref.bearingLeft  = (metrics.horiBearingX / 72.0)
        ref.bearingRight = (metrics.horiAdvance - metrics.width - metrics.horiBearingX) / 72.0
        ref.ascent       = (metrics.horiBearingY / 72.0)
        ref.descent      = (metrics.height - metrics.horiBearingY) / 72.0

        dirty = true
        atlas.dirty = true

        return ref
    }
}