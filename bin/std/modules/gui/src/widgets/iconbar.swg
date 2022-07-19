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
    iconPadding:    f32 = 4
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

        pad := 2*iconPadding
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
        pos.height = #self.iconSize
        res := Wnd.create'IconBar(parent, null, pos, id)
        res.setWndFlags(.DispatchToChildsUI)
        res.iconSize = #self.iconSize
        return res
    }

    // Add a new button
    mtd addItem(icon: Icon, id: WndId, iconBtnFlags = IconButtonFlags.None)
    {
        iconT := icon
        iconT.sizeX, iconT.sizeY = iconSize
        size := iconSize + 2*iconPadding
        b := IconButton.create(self, iconT, @{0, 0, size, size})
        b.iconBtnFlags = iconBtnFlags
        b.id = #self.id
        b.dockStyle = .Left
        computeLayout()
        updateCommandState()
    }

    // Change bar flags
    mtd setIconBarFlags(add, remove = IconBarFlags.None)
    {
        iconBarFlags |= add
        iconBarFlags &= ~remove
        computeLayout()
    }
}