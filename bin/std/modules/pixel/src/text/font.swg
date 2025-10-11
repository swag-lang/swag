using Core, FreeType

public struct GlyphAtlas
{
    texture:     Texture
    width:       u32
    height:      u32
    bpp:         u32
    datas:       Array'u8
    dirty:       bool
    curX:        s32
    curY:        s32
    nextY:       s32
}

public struct GlyphDesc
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

public struct Font
{
    ascent:            f32
    descent:           f32
    height:            f32
    underlinePos:      f32
    underlineSize:     f32
    forceBold:         bool
    forceItalic:       bool

    fullname:          String
    typeFace:          *TypeFace
    size:              u32
    atlases:           Array'GlyphAtlas
    asciiRef:          [128] GlyphDesc
    unicodeRef:        HashTable'(rune, *GlyphDesc)
    dirty:             bool
}

internal var g_HashFont: HashTable'(String, *Font)

#drop
{
    foreach k, v in g_HashFont do
        Memory.delete(v)
    g_HashFont.free()
}

public impl Font
{
    internal var lock: Sync.Mutex

    mtd opDrop()
    {
        foreach k, v in .unicodeRef do
            Memory.delete(v)
    }

    internal func mulFix(a, b: s32)->s32
    {
        let ab = cast(s64) a * cast(s64) b
        return cast(s32) ((ab + 0x8000 - (ab < 0 ? 1 : 0)) >> 16)
    }

    // Clear internal caches
    func clearCaches()
    {
        foreach a, b in g_HashFont do
            Memory.delete(b)
        g_HashFont.clear()
    }

    // Release the given font
    func release(font: *Font)
    {
        if !font do
            return
        Sync.scopedLock(&lock)
        g_HashFont.remove(font.fullname)
        Memory.delete(font)
    }

    // Creates a new Font of the given typeface and size
    #[Swag.Overload]
    func create(typeFace: *TypeFace, size: u32, forceBold, forceItalic = false)->*Font
    {
        Debug.assert(typeFace != null)
        Debug.assert(size > 0)

        let str = Format.toString("%_%_%_%", typeFace.fullname, size, forceBold, forceItalic)

        Sync.scopedLock(&lock)

        let here = g_HashFont.tryFind(str)
        if here do
            return here.value

        let font = Memory.new'Font()
        font.fullname    = str
        font.typeFace    = typeFace
        font.size        = size
        font.forceBold   = forceBold
        font.forceItalic = forceItalic

        assume FT_Set_Pixel_Sizes(typeFace.face, 0, size)
        let metrics = &typeFace.face.size.metrics
        font.ascent        = metrics.ascender / 64.0
        font.descent       = metrics.descender / 64.0
        font.height        = font.ascent - font.descent
        font.underlinePos  = mulFix(cast(s32) typeFace.face.underline_position, metrics.y_scale) / 64.0
        font.underlineSize = mulFix(cast(s32) typeFace.face.underline_thickness, metrics.y_scale) / 64.0

        g_HashFont.add(str, font)
        return font
    }

    // Creates a new Font with the given filename and size
    #[Swag.Overload]
    func create(fullname: string, size: u32, forceBold, forceItalic = false)->*Font throw
    {
        Debug.assert(@countof(fullname) != 0)
        Debug.assert(size > 0)

        let typeFace = try TypeFace.load(fullname)
        return create(typeFace, size, forceBold, forceItalic)
    }

    // Returns the glyph descriptor of a given rune
    mtd getGlyphDesc(c: rune)->const *GlyphDesc
    {
        // Already there ?
        if c < 128
        {
            if .asciiRef[cast(u32) c].atlasIndex >= 0 do
                return &.asciiRef[cast(u32) c]
        }
        else
        {
            let entry = .unicodeRef.tryFind(c)
            if entry do
                return entry.value
        }

        // Render the glyph
        let bmp = assume .typeFace.renderGlyph(c, .size, .forceBold, .forceItalic)

        // Creates a new atlas if this is the first time, or if the last atlas is full
        var atlas = .atlases.count ? &.atlases.buffer[.atlases.count - 1] : null
        if atlas
        {
            if atlas.curX + bmp.width > atlas.width
            {
                atlas.curX = 0
                if atlas.nextY + bmp.rows > atlas.height do
                    atlas = null
                else do
                    atlas.curY = atlas.nextY
            }
            elif atlas.curY + bmp.rows > atlas.height
            {
                atlas = null
            }
        }

        if !atlas
        {
            var newAtlas: GlyphAtlas
            newAtlas.bpp    = 1
            newAtlas.width  = Math.roundUpToPowerOf2(.size * 4) * newAtlas.bpp
            newAtlas.height = Math.roundUpToPowerOf2(.size * 4)
            newAtlas.datas.resize(cast(u64) newAtlas.width * newAtlas.height)
            Memory.clear(newAtlas.datas.buffer, newAtlas.datas.count)
            .atlases.add(newAtlas)
            atlas = &.atlases.buffer[.atlases.count - 1]
        }

        let x = atlas.curX
        let y = atlas.curY
        atlas.curX += bmp.width
        atlas.nextY = Math.max(atlas.nextY, atlas.curY + bmp.rows)

        // Copy the rendered glyph to the atlas datas
        var buffer = atlas.datas.buffer + x + (atlas.width * (atlas.height - 1))
        buffer -= (y * atlas.width)
        var src = bmp.buffer
        for j in bmp.rows
        {
            Memory.copy(buffer, src, cast(u64) bmp.width)
            src += bmp.pitch
            buffer -= atlas.width
        }

        // Setup GlyphDesc
        var refr: *GlyphDesc
        if c < 128 do
            refr = &.asciiRef[cast(u32) c]
        else
        {
            refr = Memory.new'GlyphDesc()
            .unicodeRef.add(c, refr)
        }

        refr.width  = bmp.width
        refr.height = bmp.rows

        let glyph = .typeFace.face.glyph
        refr.advanceX = glyph.advance.x / 64.0
        refr.advanceY = glyph.advance.y / 64.0
        refr.shiftX   = glyph.bitmap_left
        refr.shiftY   = glyph.bitmap_top

        let widthf  = cast(f32) atlas.width
        let heightf = cast(f32) atlas.height
        refr.uv.x      = x / widthf
        refr.uv.y      = y / heightf
        refr.uv.width  = refr.width / widthf
        refr.uv.height = refr.height / heightf
        refr.width /= atlas.bpp

        refr.atlasIndex = cast(u32) (.atlases.count - 1)

        .dirty      = true
        atlas.dirty = true

        return refr
    }
}
