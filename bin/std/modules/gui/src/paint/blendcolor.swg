#global public
using Pixel

// Used to animate the blending of a color.
struct BlendColor
{
    curVec4:       Math.Vector4
    startVec4:     Math.Vector4
    destVec4:      Math.Vector4
    factor:        f32
    start:         bool = true
    speed:         f32 = 8
}

impl BlendColor
{
    // Apply the color lerp to reach 'target'
    mtd apply(wnd: *Wnd, target: Color)
    {
        if start
        {
            start = false
            curVec4, startVec4, destVec4 = target.toVector4()
            return
        }

        var vec4 = target.toVector4()
        if destVec4 != vec4
        {
            startVec4 = curVec4
            destVec4  = vec4
            factor    = 0
        }
        elif factor >= 1.0
        {
            curVec4, startVec4, destVec4 = target.toVector4()
            return
        }

        var startV = startVec4
        var destV  = destVec4
        if startV.w == 0
        {
            startV.x = destVec4.x
            startV.y = destVec4.y
            startV.z = destVec4.z
        }

        if destV.w == 0
        {
            destV.x = startVec4.x
            destV.y = startVec4.y
            destV.z = startVec4.z
        }

        curVec4 = startV.lerpTo(destV, factor)
        factor += wnd.getApp().getDt() * speed
        wnd.invalidate()
    }

    // Reset the blend
    mtd reset()
    {
        start = true
    }

    // Get the current color
    mtd cur()->Color
    {
        return Color.fromVector4(curVec4)
    }
}
