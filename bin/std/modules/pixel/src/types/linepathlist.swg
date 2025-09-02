#global public
using Core, Core.Math

struct LinePathList
{
    paths:     Array'(*LinePath)
    bvMin:     Vector2
    bvMax:     Vector2
}

impl LinePathList
{
    mtd opDrop()
    {
        me.clear()
    }

    mtd opPostCopy()
    {
        foreach p, i in me.paths
        {
            var ptr = Memory.new'LinePath()
            dref ptr    = dref p
            me.paths[i] = ptr
        }
    }

    #[Swag.Inline]
    mtd count() => me.paths.count

    // Remove all internal line paths
    mtd clear()
    {
        foreach p in me.paths:
            Memory.delete(p)
        me.paths.clear()
    }

    // Returns a new path
    mtd newPath()->*LinePath
    {
        let ptr = Memory.new'LinePath()
        me.paths.add(ptr)
        return ptr
    }

    // Flatten all paths
    mtd flatten(quality: PaintQuality)
    {
        // Flatten all path, and compute BV
        me.bvMin = Vector2.Max
        me.bvMax = Vector2.NegMax
        foreach p in me.paths
        {
            p.flatten(quality)
            me.bvMin = Vector2.min(me.bvMin, p.bvMin)
            me.bvMax = Vector2.max(me.bvMax, p.bvMax)
        }
    }

    // Call fast offset on all paths
    // They must before be flattened
    #[Swag.Overload]
    mtd offset(value: f32)
    {
        if Math.isZeroEpsilon(value):
            return

        foreach p in me.paths
        {
            let v = p.polygon.isClockwise ? value : -value
            p.polygon.offset(v)
        }
    }

    // Call quality offset on all paths
    // They must before be flattened
    #[Swag.Overload]
    mtd offset(value: f32, joinStyle: JoinStyle, toler: f32 = 0.5)
    {
        if Math.isZeroEpsilon(value):
            return

        var newList: LinePathList

        foreach p in me.paths
        {
            let v = p.polygon.isClockwise ? value : -value

            var res = p.polygon.getOffset(v, joinStyle, toler)
            newList.paths.add(res.paths.toSlice())
            res.paths.count = 0
        }

        me.clear()
        me.paths = #move newList.paths
        me.clean()
    }

    // Clean all paths
    // They must before be flattened
    mtd clean()
    {
        var newList: LinePathList

        foreach p in me.paths
        {
            var res = p.polygon.getClean()
            newList.paths.add(res.paths.toSlice())
            res.paths.count = 0
        }

        me.clear()
        me.paths = #move newList.paths
    }
}
