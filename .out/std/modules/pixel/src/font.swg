#global public
using Core, FreeType

struct GlyphAtlas
{
    texture:    Texture
    width:      u32
    height:     u32
    bpp:        u32
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
    ascent:         f32
    descent:        f32
    height:         f32
    underlinePos:   f32
    underlineSize:  f32

    typeFace:   const *TypeFace
    size:       u32
    atlases:    Array'GlyphAtlas
    asciiRef:   [128] GlyphDesc
    unicodeRef: HashTable'(rune, *GlyphDesc)
    dirty:      bool
}

private var g_HashFont: HashTable'(String, *Font)
private var g_Lock:     Sync.Mutex

impl Font
{
    private func mulFix(a, b: s32)->s32
    {
        ab := cast(s64) a * cast(s64) b
        return cast(s32) (((ab + 0x8000 - (ab < 0 ? 1 : 0)) >>,safe 16))
    }

    // Creates a new Font of the given typeface and size
    func create(typeFace: const *TypeFace, size: u32)->*Font
    {
        Debug.assert(typeFace != null)
        Debug.assert(size > 0)

        str := Format.toString("%_%", typeFace.fullname, size)

        Sync.scopedLock(&g_Lock)

        here := g_HashFont.find(str)
        if here return here.value

        font := Memory.new'Font()
        font.typeFace = typeFace
        font.size = size

        assume FT_Set_Pixel_Sizes(typeFace.face, 0, size)
        metrics := &typeFace.face.size.metrics
        font.ascent  = metrics.ascender / 64.0
        font.descent = metrics.descender / 64.0
        font.height  = font.ascent - font.descent
        font.underlinePos = mulFix(cast(s32) typeFace.face.underline_position, metrics.y_scale) / 64.0
        font.underlineSize = mulFix(cast(s32) typeFace.face.underline_thickness, metrics.y_scale) / 64.0

        g_HashFont.add(str, font)
        return font
    }

    // Creates a new Font with the given filename and size
    func create(fullname: string, size: u32)->*Font throw
    {
        Debug.assert(@countof(fullname) != 0)
        Debug.assert(size > 0)

        typeFace := try TypeFace.create(fullname)
        return create(typeFace, size)
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

        bpp := 1
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
            newAtlas.bpp = bpp
            newAtlas.width =  Math.roundUpToPowerOf2(size * 4) * newAtlas.bpp
            newAtlas.height = Math.roundUpToPowerOf2(size * 4)
            newAtlas.datas.resize(cast(uint) newAtlas.width * newAtlas.height)
            Memory.clear(newAtlas.datas.buffer, newAtlas.datas.count)
            atlases.add(newAtlas)
            atlas = atlases[atlases.count - 1]
        }

        x := atlas.curX
        y := atlas.curY
        atlas.curX += bmp.width
        atlas.nextY = Math.max(atlas.nextY, atlas.curY + bmp.rows)

        // Copy the rendered glyph to the atlas datas
        buffer := atlas.datas.buffer + x + (atlas.width * (atlas.height - 1))
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
        heightf := cast(f32) atlas.height
        ref.uv.x = x / widthf
        ref.uv.y = y / heightf
        ref.uv.width = ref.width / widthf
        ref.uv.height = ref.height / heightf
        ref.width /= atlas.bpp

        ref.atlasIndex = cast(u32) (atlases.count - 1)

        dirty = true
        atlas.dirty = true

        return ref
    }
}