#global public
using Core, FreeType

protected var g_FT: FT_Library
#init
{
    catch FT_Init_FreeType(&g_FT)
}

struct TypeFace
{
    fullname:   string
    buffer:     Array'u8
    face:       FT_Face
}

private var g_HashTypeFace: HashTable'(String, *TypeFace)
private var g_Lock:         Sync.Mutex

impl TypeFace
{
    // Get or load a new typeface
    func create(fullname: string)->*TypeFace throw
    {
        Sync.scopedLock(&g_Lock)

        str := String.from(fullname)
        here := g_HashTypeFace.find(str)
        if here return here.value

        tf := Memory.new'TypeFace()
        tf.fullname = fullname
        tf.buffer = try File.readAllBytes(fullname)
        try FT_New_Memory_Face(g_FT, tf.buffer.buffer, cast(FT_Long) tf.buffer.count, 0, &tf.face)

        g_HashTypeFace.add(str, tf)
        return tf
    }

    #[Swag.callback]
    private func moveTo(to: const *FT_Vector, pathList: *LinePathList)->s32
    {
        path := pathList.newPath()
        path.start(cast(f32) to.x / 64.0, cast(f32) -to.y / 64.0)
        return 0
    }

    #[Swag.callback]
    private func lineTo(to: const *FT_Vector, pathList: *LinePathList)->s32
    {
        path := pathList.paths[pathList.paths.count - 1]
        path.lineTo(cast(f32) to.x / 64.0, cast(f32) -to.y / 64.0)
        return 0
    }

    #[Swag.callback]
    private func conicTo(ctrl, to: const *FT_Vector, pathList: *LinePathList)->s32
    {
        path := pathList.paths[pathList.paths.count - 1]
        path.bezierTo(cast(f32) to.x / 64.0, cast(f32) -to.y / 64.0, cast(f32) ctrl.x / 64.0, cast(f32) -ctrl.y / 64.0)
        return 0
    }

    #[Swag.callback]
    private func bezierTo(ctrl1, ctrl2, to: const *FT_Vector, pathList: *LinePathList)->s32
    {
        path := pathList.paths[pathList.paths.count - 1]
        path.bezierTo(cast(f32) to.x / 64.0, cast(f32) -to.y / 64.0, cast(f32) ctrl1.x / 64.0, cast(f32) -ctrl1.y / 64.0, cast(f32) ctrl2.x / 64.0, cast(f32) -ctrl2.y / 64.0)
        return 0
    }

    // Convert a string to its outline, at a given size
    func getStringOutline(using const self, pathList: *LinePathList, text: string, size: u32)
    {
        var itf: FT_Outline_Funcs
        itf.move_to  = @mkcallback(&moveTo)
        itf.line_to  = @mkcallback(&lineTo)
        itf.conic_to = @mkcallback(&conicTo)
        itf.cubic_to = @mkcallback(&bezierTo)

        assume FT_Set_Pixel_Sizes(face, 0, size)

        serial := 0
        x := 0

        Utf8.visitRunes(|c| text)
        {
            assume FT_Load_Char(face, cast(FT_ULong) c, FT_LOAD_NO_BITMAP)

            x1 := x + (face.glyph.bitmap_left * 64)
            FT_Outline_Translate(face.glyph.outline, x1, 0)

            prevIdx := pathList.paths.count
            assume FT_Outline_Decompose(&face.glyph.outline, &itf, pathList)

            if prevIdx != pathList.paths.count
            {
                loop i: prevIdx..pathList.paths.count-1
                    pathList.paths[i].serial = serial
                serial += 1
            }

            x += face.glyph.advance.x
        }
    }
}