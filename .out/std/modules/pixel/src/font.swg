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
    advanceX:       f32
    advanceY:       f32
    shiftX:         f32
    shiftY:         f32
    width:          f32
    height:         f32
}

struct Font
{
    typeFace:   const *TypeFace
    size:       u32
    atlases:    Array'GlyphAtlas
    asciiRef:   [128] GlyphRef
    unicodeRef: HashTable'(rune, GlyphRef)
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
            entry := unicodeRef.find(c)
            if entry
                return entry.value
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
        {
            var refC: GlyphRef
            unicodeRef.add(c, refC)
            entry := unicodeRef.find(c)
            ref = &entry.value
        }

        ref.width  = bmp.width
        ref.height = bmp.rows

        glyph := typeFace.face.glyph
        ref.advanceX = glyph.advance.x / 64.0
        ref.advanceY = glyph.advance.y / 64.0
        ref.shiftX   = glyph.bitmap_left
        ref.shiftY   = glyph.bitmap_top

        ref.uv.x = (x * size) / cast(f32) atlas.width
        ref.uv.y = (y * size) / cast(f32) atlas.width
        ref.uv.width = ref.width / cast(f32) atlas.width
        ref.uv.height = ref.height / cast(f32) atlas.width

        ref.atlasIndex = cast(u32) (atlases.count - 1)


        dirty = true
        atlas.dirty = true

        return ref
    }
}