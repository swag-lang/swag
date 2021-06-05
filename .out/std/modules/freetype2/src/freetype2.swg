#global export

alias FT_Library   = *FT_LibraryRec_
alias FT_Memory    = *void
alias FT_Module    = *void
alias FT_Renderer  = *void
alias FT_DebugHook_Func = *void


alias FT_Int    = s32
alias FT_UInt   = u32
alias FT_Long   = s32
alias FT_ULong  = u32
alias FT_Pos    = s32
alias FT_Error  = s32

const FT_MAX_MODULES = 32

struct FT_LibraryRec_
{
    memory:             FT_Memory
    version_major:      FT_Int
    version_minor:      FT_Int
    version_patch:      FT_Int
    num_modules:        FT_UInt
    modules:            [FT_MAX_MODULES] FT_Module
    renderers:          FT_ListRec
    cur_renderer:       FT_Renderer
    auto_hinter:        FT_Module
    debug_hooks:        [4] FT_DebugHook_Func
    lcd_geometry:       [3] FT_Vector
    refcount:           FT_Int
}

alias FT_ListNode = *void
struct FT_ListRec
{
    head: FT_ListNode
    tail: FT_ListNode
}

struct FT_Vector
{
    x:  FT_Pos
    y:  FT_Pos
}

#[Swag.foreign("freetype2")]
{
    func FT_Init_FreeType(alibrary: *FT_Library)->FT_Error;
    func FT_Done_FreeType(alibrary: FT_Library)->FT_Error;
}