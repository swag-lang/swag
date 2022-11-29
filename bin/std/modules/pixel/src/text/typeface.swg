using Core, FreeType

var g_FT: FT_Library
#init
{
    catch FT_Init_FreeType(&g_FT)
}

public struct TypeFace
{
    fullname:       string
    buffer:         Array'u8
    face:           FT_Face
    forceItalic:   bool
}

impl TypeFace
{
    var hashTypeFace: HashTable'(String, *TypeFace)
    var lock:         Sync.Mutex

    func moveTo(to: const *FT_Vector, pathList: *LinePathList)->s32
    {
        path := pathList.newPath()
        path.start(cast(f32) to.x / 64.0, cast(f32) -to.y / 64.0)
        return 0
    }

    func lineTo(to: const *FT_Vector, pathList: *LinePathList)->s32
    {
        path := pathList.paths[pathList.paths.count - 1]
        path.lineTo(cast(f32) to.x / 64.0, cast(f32) -to.y / 64.0)
        return 0
    }

    func conicTo(ctrl, to: const *FT_Vector, pathList: *LinePathList)->s32
    {
        path := pathList.paths[pathList.paths.count - 1]
        path.bezierTo(cast(f32) to.x / 64.0, cast(f32) -to.y / 64.0, cast(f32) ctrl.x / 64.0, cast(f32) -ctrl.y / 64.0)
        return 0
    }

    func bezierTo(ctrl1, ctrl2, to: const *FT_Vector, pathList: *LinePathList)->s32
    {
        path := pathList.paths[pathList.paths.count - 1]
        path.bezierTo(cast(f32) to.x / 64.0, cast(f32) -to.y / 64.0, cast(f32) ctrl1.x / 64.0, cast(f32) -ctrl1.y / 64.0, cast(f32) ctrl2.x / 64.0, cast(f32) -ctrl2.y / 64.0)
        return 0
    }
}

public impl TypeFace
{
    // Returns the underlying font name
    mtdc getFamilyName()->string
    {
        defname := @mkstring(face.family_name, @strlen(face.family_name))
        return defname
    }

    // Create a new typeface
    func create(fullname: string, bytes: const [..] u8)->*TypeFace throw
    {
        tf := Memory.new'TypeFace()
        tf.fullname = fullname
        tf.buffer = bytes
        try FT_New_Memory_Face(g_FT, tf.buffer.buffer, cast(FT_Long) tf.buffer.count, 0, &tf.face)
        return tf
    }

    // Get or load a new typeface
    func load(fullname: string)->*TypeFace throw
    {
        Sync.scopedLock(&lock)

        str := String.from(fullname)
        here := hashTypeFace.find(str)
        if here
            return here.value

        tf := Memory.new'TypeFace()
        tf.fullname = fullname

        // If file does not exist, try the system folder
        if !File.exists(fullname)
        {
            p := Env.getSpecialDirectory(.Font)
            p += "/"
            p += fullname
            tf.buffer = try File.readAllBytes(p)
        }
        else
        {
            tf.buffer = try File.readAllBytes(fullname)
        }

        try FT_New_Memory_Face(g_FT, tf.buffer.buffer, cast(FT_Long) tf.buffer.count, 0, &tf.face)

        hashTypeFace.add(str, tf)
        return tf
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

        try FT_Set_Pixel_Sizes(face, 0, size)

        try FT_Load_Char(face, cast(FT_ULong) c, FT_LOAD_NO_BITMAP)
        if embolden
            try FT_Outline_Embolden(&face.glyph.outline, cast(FT_Pos) embolden * 64)
        if forceItalic
            FT_GlyphSlot_Oblique(face.glyph)

        prevIdx := pathList.paths.count
        try FT_Outline_Decompose(&face.glyph.outline, &itf, pathList)

        if prevIdx != pathList.paths.count
        {
            loop i: prevIdx..pathList.paths.count-1
                pathList.paths[i].isClosed = true
        }

        return embolden + (face.glyph.advance.x / 64.0)
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

        try FT_Set_Pixel_Sizes(face, 0, size)

        serial := 0
        x := 0

        Utf8.visitRunes(|c| text)
        {
            try FT_Load_Char(face, cast(FT_ULong) c, FT_LOAD_NO_BITMAP)
            if embolden
                try FT_Outline_Embolden(&face.glyph.outline, cast(FT_Pos) embolden * 64)
            if forceItalic
                FT_GlyphSlot_Oblique(face.glyph)

            FT_Outline_Translate(face.glyph.outline, x, 0)

            prevIdx := pathList.paths.count
            try FT_Outline_Decompose(&face.glyph.outline, &itf, pathList)

            if prevIdx != pathList.paths.count
            {
                loop i: prevIdx..pathList.paths.count-1
                {
                    pathList.paths[i].isClosed = true
                    pathList.paths[i].serial = serial
                }

                serial += 1
            }

            x += face.glyph.advance.x
        }
    }
}