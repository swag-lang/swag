using Core, FreeType

var g_FT: FT_Library
#init
{
    catch FT_Init_FreeType(&g_FT)
}

public struct TypeFace
{
    fullname:         string
    buffer:           Array'u8
    face:             FT_Face
    forceBoldDiv:     u32 = 11
    forceItalic:      bool
    forceBold:        bool
}

var g_HashTypeFace: HashTable'(String, *TypeFace)

#drop
{
    foreach k, v in g_HashTypeFace do
        Memory.delete(v)
    g_HashTypeFace.free()
}

impl TypeFace
{
    var lock: Sync.Mutex

    func moveTo(toV: const *FT_Vector, pathList: *LinePathList)->s32
    {
        let path = pathList.newPath()
        path.start(toV.x / 64.0, -toV.y / 64.0)
        return 0
    }

    func lineTo(toV: const *FT_Vector, pathList: *LinePathList)->s32
    {
        let path = pathList.paths[pathList.paths.count - 1]
        path.lineTo(toV.x / 64.0, -toV.y / 64.0)
        return 0
    }

    func conicTo(ctrl, toV: const *FT_Vector, pathList: *LinePathList)->s32
    {
        let path = pathList.paths[pathList.paths.count - 1]
        path.bezierTo(toV.x / 64.0, -toV.y / 64.0, ctrl.x / 64.0, -ctrl.y / 64.0)
        return 0
    }

    func bezierTo(ctrl1, ctrl2, toV: const *FT_Vector, pathList: *LinePathList)->s32
    {
        let path = pathList.paths[pathList.paths.count - 1]
        path.bezierTo(toV.x / 64.0, -toV.y / 64.0, ctrl1.x / 64.0, -ctrl1.y / 64.0, ctrl2.x / 64.0, -ctrl2.y / 64.0)
        return 0
    }
}

public impl TypeFace
{
    // Returns true if the typeface is bold
    mtd const isBold()->bool
    {
        return cast(bool) (.face.style_flags & FT_STYLE_FLAG_BOLD)
    }

    // Returns true if the typeface is italic
    mtd const isItalic()->bool
    {
        return cast(bool) (.face.style_flags & FT_STYLE_FLAG_ITALIC)
    }

    // Returns the underlying font family name
    mtd const getFamilyName()->string
    {
        let defname = @mkstring(.face.family_name, @strlen(.face.family_name))
        return defname
    }

    // Release the given typeface
    func release(typeface: *TypeFace)
    {
        if !typeface do
            return
        Sync.scopedLock(&lock)
        g_HashTypeFace.remove(typeface.fullname)
        Memory.delete(typeface)
    }

    // Get or create a new typeface from a memory buffer
    func create(fullname: string, bytes: const [..] u8)->*TypeFace throw
    {
        Sync.scopedLock(&lock)

        let here = g_HashTypeFace.tryFind(fullname)
        if here do
            return here.value

        let tf = Memory.new'TypeFace()
        tf.fullname = fullname
        tf.buffer   = bytes
        FT_New_Memory_Face(g_FT, tf.buffer.buffer, cast(FT_Long) tf.buffer.count, 0, &tf.face)

        g_HashTypeFace.add(fullname, tf)
        return tf
    }

    // Get or load a new typeface
    func load(fullname: string)->*TypeFace throw
    {
        Sync.scopedLock(&lock)

        let here = g_HashTypeFace.tryFind(fullname)
        if here do
            return here.value

        let tf = Memory.new'TypeFace()
        tf.fullname = fullname

        // If file does not exist, try the system folder
        if !File.exists(fullname)
        {
            let p = Path.combine(Env.getSpecialDirectory(.Font), fullname)
            tf.buffer = File.readAllBytes(p)
        }
        else
        {
            tf.buffer = File.readAllBytes(fullname)
        }

        FT_New_Memory_Face(g_FT, tf.buffer.buffer, cast(FT_Long) tf.buffer.count, 0, &tf.face)

        g_HashTypeFace.add(fullname, tf)
        return tf
    }

    internal mtd transform(size, embolden: u32 = 0, forceBold, forceItalic = false) throw
    {
        var emb = .forceBold or forceBold ? Math.max(1, size / .forceBoldDiv) : 0
        emb += embolden
        if emb do
            FT_Outline_Embolden(&.face.glyph.outline, cast(FT_Pos) emb * 64)
        if .forceItalic or forceItalic do
            FT_GlyphSlot_Oblique(.face.glyph)
    }

    // Render one glyph
    mtd renderGlyph(c: rune, size: u32, forceBold, forceItalic = false)->const *FT_Bitmap throw
    {
        FT_Set_Pixel_Sizes(.face, 0, size)
        FT_Load_Char(.face, cast(FT_ULong) c, FT_LOAD_DEFAULT)
        .transform(size, 0, forceBold, forceItalic)
        FT_Render_Glyph(.face.glyph, .FT_RENDER_MODE_NORMAL)
        return &.face.glyph.bitmap
    }

    // Convert a rune to its outline, at a given size
    #[Swag.Discardable]
    mtd getRuneOutline(pathList: *LinePathList, c: rune, size: u32, embolden: u32 = 0)->f32 throw
    {
        var itf: FT_Outline_Funcs

        pathList.clear()
        itf.move_to  = cast(*void) @mkcallback(&moveTo)
        itf.line_to  = cast(*void) @mkcallback(&lineTo)
        itf.conic_to = cast(*void) @mkcallback(&conicTo)
        itf.cubic_to = cast(*void) @mkcallback(&bezierTo)

        FT_Set_Pixel_Sizes(.face, 0, size)
        FT_Load_Char(.face, cast(FT_ULong) c, FT_LOAD_NO_BITMAP)
        .transform(size, embolden)

        let prevIdx = pathList.paths.count
        FT_Outline_Decompose(&.face.glyph.outline, &itf, pathList)

        if prevIdx != pathList.paths.count
        {
            for i in prevIdx until pathList.paths.count do
                pathList.paths[i].isClosed = true
        }

        return embolden + (.face.glyph.advance.x / 64.0)
    }

    // Convert a string to its outline, at a given size
    mtd getStringOutline(pathList: *LinePathList, text: string, size: u32, embolden: u32 = 0) throw
    {
        var itf: FT_Outline_Funcs

        pathList.clear()
        itf.move_to  = cast(*void) @mkcallback(&moveTo)
        itf.line_to  = cast(*void) @mkcallback(&lineTo)
        itf.conic_to = cast(*void) @mkcallback(&conicTo)
        itf.cubic_to = cast(*void) @mkcallback(&bezierTo)

        try FT_Set_Pixel_Sizes(.face, 0, size)

        var serial = 0
        var x      = 0

        Utf8.visitRunes(|c| text)
        {
            FT_Load_Char(.face, cast(FT_ULong) c, FT_LOAD_NO_BITMAP)
            .transform(size, embolden)

            FT_Outline_Translate(.face.glyph.outline, x, 0)

            let prevIdx = pathList.paths.count
            FT_Outline_Decompose(&.face.glyph.outline, &itf, pathList)

            if prevIdx != pathList.paths.count
            {
                for i in prevIdx until pathList.paths.count
                {
                    pathList.paths[i].isClosed = true
                    pathList.paths[i].serial   = serial
                }

                serial += 1
            }

            x += .face.glyph.advance.x
        }
    }
}
