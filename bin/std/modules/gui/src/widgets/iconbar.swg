#global public
using Core, Pixel

#[Swag.EnumFlags]
enum IconBarFlags
{
    None
    Vertical
}

struct IconBar
{
    using wnd:  Wnd

    iconBarFlags    = IconBarFlags.None
    iconSize:       f32
}

impl IconBar
{
    private mtd computeLayout()
    {
        visit c: childs
        {
            if iconBarFlags & .Vertical
                c.dockStyle = .Top
            else
                c.dockStyle = .Left
        }

        pad := 2*getThemeMetrics().iconBar_ButtonsPadding
        size := cast(f32) (childs.count * (iconSize + pad))
        if iconBarFlags & .Vertical
            resize(iconSize + pad, size)
        else
            resize(size, iconSize + pad)
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, iconSize: f32, position: Math.Rectangle = @{}, id: WndId = null)->*IconBar
    {
        Debug.assert(parent != null)
        pos := position
        pos.height = #self.iconSize + 2*parent.getThemeMetrics().iconBar_ButtonsPadding
        res := Wnd.create'IconBar(parent, null, pos, id)
        res.iconSize = #self.iconSize
        return res
    }

    // Add a new button
    mtd addItem(icon: Icon, id: WndId)
    {
        iconT := icon
        iconT.sizeX, iconT.sizeY = iconSize
        size := iconSize + 2*getThemeMetrics().iconBar_ButtonsPadding
        b := IconButton.create(self, iconT, @{0, 0, size, size})
        b.id = #self.id
        b.dockStyle = .Left
        computeLayout()
    }

    // Change bar flags
    mtd setIconBarFlags(add, remove = IconBarFlags.None)
    {
        iconBarFlags |= add
        iconBarFlags &= ~remove
        computeLayout()
    }
}