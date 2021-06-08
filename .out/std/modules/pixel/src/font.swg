#global public
using Core, FreeType

struct GlyphAtlas
{
    texture:    Texture
    width:      u32
    datas:      Array'u8
    dirty:      bool
    curX:       s32
    curY:       s32
    nextY:      s32
}

struct GlyphDesc
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
    asciiRef:   [128] GlyphDesc
    unicodeRef: HashTable'(rune, *GlyphDesc)
    dirty:      bool
}

impl Font
{
    // Creates a new Font of the given typeface and size
    func create(typeFace: const *TypeFace, size: u32)->Font
    {
        Debug.assert(typeFace != null)
        Debug.assert(size > 0)

        var result: retval
        result.typeFace = typeFace
        result.size = size
        return result
    }

    // Returns the glyph decriptor of a given rune
    func getGlyphDesc(using self, c: rune)->const *GlyphDesc
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
        bmp := typeFace.face.glyph.bitmap

        // Creates a new atlas if this is the first time, or if the last atlas is full
        atlas := atlases.count ? atlases[atlases.count - 1] : null
        if atlas
        {
            if atlas.curX + bmp.width > atlas.width
            {
                atlas.curX = 0
                if atlas.nextY + bmp.rows > atlas.width
                    atlas = null
                else
                    atlas.curY = atlas.nextY
            }
        }

        if !atlas
        {
            var newAtlas: GlyphAtlas
            newAtlas.width = Math.roundUpToPowerOf2(size * 4)
            newAtlas.datas.resize(cast(uint) newAtlas.width * newAtlas.width)
            Memory.clear(newAtlas.datas.buffer, newAtlas.datas.count)
            atlases.add(newAtlas)
            atlas = atlases[atlases.count - 1]
        }

        x := atlas.curX
        y := atlas.curY
        atlas.curX += bmp.width
        atlas.nextY = Math.max(atlas.nextY, atlas.curY + bmp.rows)

        // Copy the rendered glyph to the atlas datas
        buffer := atlas.datas.buffer + x + (atlas.width * (atlas.width - 1))
        buffer -= (y * atlas.width)
        src := bmp.buffer
        loop j: bmp.rows
        {
            Memory.copy(buffer, src, cast(uint) bmp.width)
            src += bmp.pitch
            buffer -= atlas.width
        }

        // Setup GlyphDesc
        var ref: *GlyphDesc
        if c < 128
            ref = &asciiRef[cast(u32) c]
        else
        {
            ref = Memory.new'GlyphDesc()
            unicodeRef.add(c, ref)
        }

        ref.width  = bmp.width
        ref.height = bmp.rows

        glyph := typeFace.face.glyph
        ref.advanceX = glyph.advance.x / 64.0
        ref.advanceY = glyph.advance.y / 64.0
        ref.shiftX   = glyph.bitmap_left
        ref.shiftY   = glyph.bitmap_top

        widthf := cast(f32) atlas.width
        ref.uv.x = x / widthf
        ref.uv.y = y / widthf
        ref.uv.width = ref.width / widthf
        ref.uv.height = ref.height / widthf

        ref.atlasIndex = cast(u32) (atlases.count - 1)

        dirty = true
        atlas.dirty = true

        return ref
    }
}