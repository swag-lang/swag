#global public
using Core, Pixel

#[Swag.EnumFlags]
enum IconBarFlags
{
    Zero     = 0
    Vertical
}

struct IconBar
{
    using wnd:           Wnd

    iconBarFlags         = IconBarFlags.Zero
    iconSize:            f32
    iconPadding:         f32 = 4

    sigUpdateState:      SigArray'func||(*CommandStateEvent)
    sigCheckChanged:     SigArray'func||(*IconButton)
    sigPressed:          SigArray'func||(*IconButton)
}

impl IWnd for IconBar
{
    mtd impl onComputeStateEvent(evt: *CommandStateEvent)
    {
        evt.accepted = false
        me.sigUpdateState.call(evt)
    }
}

impl IconBar
{
    mtd computeLayout()
    {
        foreach c in me.childs
        {
            if me.iconBarFlags.has(.Vertical):
                c.dockStyle = .Top
            else:
                c.dockStyle = .Left
        }

        let pad  = 2 * me.iconPadding
        let size = cast(f32) (me.childs.count * (me.iconSize + pad))
        if me.iconBarFlags.has(.Vertical):
            me.resize(me.iconSize + pad, size)
        else:
            me.resize(size, me.iconSize + pad)
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, iconSize: f32, position: Math.Rectangle = {}, id: WndId = null)->*IconBar
    {
        Debug.assert(parent != null)
        var pos = position
        pos.height = iconSize
        let res = Wnd.create'IconBar(parent, null, pos, id)
        res.iconSize = iconSize
        return res
    }

    // Add a new button
    #[Swag.Discardable]
    mtd addItem(icon: Icon, btnId: WndId = null, iconBtnFlags = IconButtonFlags.Zero)->*IconButton
    {
        var iconT = icon
        iconT.sizeX, iconT.sizeY = me.iconSize
        let size = me.iconSize + 2 * me.iconPadding
        let b    = IconButton.create(me, iconT, {0, 0, size, size})
        b.iconBtnFlags.set(iconBtnFlags)
        b.id        = btnId
        b.dockStyle = .Left
        b.setForm(.RoundSquare)
        me.computeLayout()

        if iconBtnFlags.has(.Checkable | .CheckableToggle)
        {
            b.sigCheckChanged += mtd||(btn)
            {
                me.sigCheckChanged.call(btn)
            }
        }
        else
        {
            b.sigPressed += mtd||(btn)
            {
                me.sigPressed.call(cast(*IconButton) btn)
            }
        }

        me.updateCommandState()
        return b
    }

    // Add a new checkable button
    #[Swag.Discardable]
    mtd addCheckableItem(icon: Icon, btnId: WndId = null)->*IconButton
    {
        return me.addItem(icon, btnId, .Checkable)
    }

    // Add a new checkable button
    #[Swag.Discardable]
    mtd addCheckableToggleItem(icon: Icon, btnId: WndId = null)->*IconButton
    {
        return me.addItem(icon, btnId, .CheckableToggle)
    }

    // Change bar flags
    mtd setIconBarFlags(add, remove = IconBarFlags.Zero)
    {
        me.iconBarFlags.add(add)
        me.iconBarFlags.remove(remove)
        me.computeLayout()
    }

    // Returns the button with the given id
    mtd getButton(btnId: string)->#null *IconButton
    {
        let btn = me.getChildById(btnId)
        if !btn:
            return null
        return cast(*IconButton) btn
    }
}
