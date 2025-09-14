#global public
using Pixel

// Used to animate the blending of a color.
struct BlendColor
{
    curVec4:       Math.Vector4
    startVec4:     Math.Vector4
    destVec4:      Math.Vector4
    factor:        f32
    speed:         f32 = 8
    themeCrc:      u32 = Swag.U32.Max
}

impl BlendColor
{
    // Apply the color lerp to reach 'target'
    mtd apply(wnd: *Wnd, target: Color)
    {
        if wnd.getApp().dirtyTheme != me.themeCrc
        {
            me.themeCrc = wnd.getApp().dirtyTheme
            me.curVec4, me.startVec4, me.destVec4 = target.toVector4()
            me.factor = 1.0
            return
        }

        let vec4 = target.toVector4()
        if me.destVec4 != vec4
        {
            me.startVec4 = me.curVec4
            me.destVec4  = vec4
            me.factor    = 0
        }
        elif me.factor >= 1.0
        {
            me.curVec4, me.startVec4, me.destVec4 = target.toVector4()
            return
        }

        var startV = me.startVec4
        var destV  = me.destVec4
        if startV.w == 0
        {
            startV.x = me.destVec4.x
            startV.y = me.destVec4.y
            startV.z = me.destVec4.z
        }

        if destV.w == 0
        {
            destV.x = me.startVec4.x
            destV.y = me.startVec4.y
            destV.z = me.startVec4.z
        }

        me.curVec4 = startV.lerpTo(destV, me.factor)
        me.factor += wnd.getApp().getDt() * me.speed
        wnd.invalidate()
    }

    // Reset the blend
    mtd reset()
    {
        me.themeCrc = Swag.U32.Max
    }

    // Get the current color
    mtd cur()->Color
    {
        return Color.fromVector4(me.curVec4)
    }
}
