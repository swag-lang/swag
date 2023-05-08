#global public
using Core, Pixel

#[Swag.EnumFlags]
enum IconBarFlags
{
    Zero = 0
    Vertical
}

struct IconBar
{
    using wnd:  Wnd

    iconBarFlags    = IconBarFlags.Zero
    iconSize:       f32
    iconPadding:    f32 = 4

    sigUpdateState:     SigArray'closure(*CommandStateEvent)
    sigCheckChanged:    SigArray'closure(*IconButton)
}

impl IWnd for IconBar
{
    mtd onComputeStateEvent(evt: *CommandStateEvent)
    {
        sigUpdateState.call(evt)
    }
}

impl IconBar
{
    private mtd computeLayout()
    {
        visit c: childs
        {
            if iconBarFlags.has(.Vertical)
                c.dockStyle = .Top
            else
                c.dockStyle = .Left
        }

        pad := 2*iconPadding
        size := cast(f32) (childs.count * (iconSize + pad))
        if iconBarFlags.has(.Vertical)
            resize(iconSize + pad, size)
        else
            resize(size, iconSize + pad)
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, iconSize: f32, position: Math.Rectangle = {}, id: WndId = null)->*IconBar
    {
        Debug.assert(parent != null)
        pos := position
        pos.height = #self.iconSize
        res := Wnd.create'IconBar(parent, null, pos, id)
        res.iconSize = #self.iconSize
        return res
    }

    // Add a new button
    #[Swag.Discardable]
    mtd addItem(icon: Icon, btnId: WndId = null, iconBtnFlags = IconButtonFlags.Zero)->*IconButton
    {
        iconT := icon
        iconT.sizeX, iconT.sizeY = iconSize
        size := iconSize + 2*iconPadding
        b := IconButton.create(self, iconT, {0, 0, size, size})
        b.iconBtnFlags.set(iconBtnFlags)
        b.id = btnId
        b.dockStyle = .Left
        computeLayout()

        if iconBtnFlags.has(.Checkable | .CheckableToggle)
        {
            b.sigCheckChanged += closure|self|(btn) {
                self.sigCheckChanged.call(btn)
                self.updateCommandState()
            }
        }

        updateCommandState()
        return b
    }

    // Add a new checkable button
    #[Swag.Discardable]
    mtd addCheckableItem(icon: Icon, btnId: WndId = null)->*IconButton
    {
        return addItem(icon, btnId, .Checkable)
    }

    // Add a new checkable button
    #[Swag.Discardable]
    mtd addCheckableToggleItem(icon: Icon, btnId: WndId = null)->*IconButton
    {
        return addItem(icon, btnId, .CheckableToggle)
    }

    // Change bar flags
    mtd setIconBarFlags(add, remove = IconBarFlags.Zero)
    {
        iconBarFlags.add(add)
        iconBarFlags.remove(remove)
        computeLayout()
    }
}