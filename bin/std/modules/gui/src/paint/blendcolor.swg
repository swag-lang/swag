#global public
using Pixel

struct BlendColor
{
    curVec4:    Math.Vector4
    startVec4:  Math.Vector4
    destVec4:   Math.Vector4
    factor:     f32
    start:      bool = true
    speed:      f32 = 6
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

        vec4 := target.toVector4()
        if destVec4 != vec4
        {
            startVec4 = curVec4
            destVec4 = vec4
            factor = 0
        }
        else if factor >= 1.0
        {
            curVec4, startVec4, destVec4 = target.toVector4()
            return
        }

        startV := startVec4
        destV  := destVec4
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

        curVec4 = Math.Vector4.lerp(startV, destV, factor)
        factor += wnd.getApp().getDt() * speed
        wnd.invalidate()
    }

    mtd cur()->Color
    {
        return Color.fromVector4(curVec4)
    }
}