#global public
using Core, FreeType

protected var g_FT: FT_Library
#init
{
    catch FT_Init_FreeType(&g_FT)
}

struct TypeFace
{
    buffer: Array'u8
    face:   FT_Face
}

impl TypeFace
{
    // Load a new font
    func load(fullname: string, faceIndex: u32 = 0)->TypeFace throw
    {
        var result: retval
        result.buffer = try File.readAllBytes(fullname)
        try FT_New_Memory_Face(g_FT, result.buffer.buffer, cast(FT_Long) result.buffer.count, cast(FT_Long) faceIndex, &result.face)
        return result
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
    func getStringOutline(using self, pathList: *LinePathList, text: string, size: u32)
    {
        var itf: FT_Outline_Funcs
        itf.move_to = @mkcallback(&moveTo)
        itf.line_to = @mkcallback(&lineTo)
        itf.conic_to = @mkcallback(&conicTo)
        itf.cubic_to = @mkcallback(&bezierTo)

        assume FT_Set_Pixel_Sizes(face, 0, size)

        x := 0
        Utf8.visitRunes(|c| text)
        {
            assume FT_Load_Char(face, cast(FT_ULong) c, FT_LOAD_NO_BITMAP)

            x1 := (x + face.glyph.bitmap_left) * 64
            FT_Outline_Translate(face.glyph.outline, x1, 0)
            assume FT_Outline_Decompose(&face.glyph.outline, &itf, pathList)

            x += face.glyph.advance.x / 64
        }
    }
}