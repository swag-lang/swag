#global public
using Pixel

struct BlendColor
{
    curVec4:    Math.Vector4
    startVec4:  Math.Vector4
    destVec4:   Math.Vector4
    factor:     f32
    start:      bool = true
    speed:      f32 = 5
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

        curVec4 = Math.Vector4.lerp(startVec4, destVec4, factor)
        factor += wnd.getApp().getDt() * speed
        wnd.invalidate()
    }

    mtd cur()->Color
    {
        return Color.fromVector4(curVec4)
    }
}