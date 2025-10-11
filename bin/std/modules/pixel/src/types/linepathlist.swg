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
        .clear()
    }

    mtd opPostCopy()
    {
        foreach p, i in .paths
        {
            var ptr = Memory.new'LinePath()
            dref ptr  = dref p
            .paths[i] = ptr
        }
    }

    #[Swag.Inline]
    mtd count() => .paths.count

    // Remove all internal line paths
    mtd clear()
    {
        foreach p in .paths do
            Memory.delete(p)
        .paths.clear()
    }

    // Returns a new path
    mtd newPath()->*LinePath
    {
        let ptr = Memory.new'LinePath()
        .paths.add(ptr)
        return ptr
    }

    // Flatten all paths
    mtd flatten(quality: PaintQuality)
    {
        // Flatten all path, and compute BV
        .bvMin = Vector2.Max
        .bvMax = Vector2.NegMax
        foreach p in .paths
        {
            p.flatten(quality)
            .bvMin = Vector2.min(.bvMin, p.bvMin)
            .bvMax = Vector2.max(.bvMax, p.bvMax)
        }
    }

    // Call fast offset on all paths
    // They must before be flattened
    #[Swag.Overload]
    mtd offset(value: f32)
    {
        if Math.isZeroEpsilon(value) do
            return

        foreach p in .paths
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
        if Math.isZeroEpsilon(value) do
            return

        var newList: LinePathList

        foreach p in .paths
        {
            let v = p.polygon.isClockwise ? value : -value

            var res = p.polygon.getOffset(v, joinStyle, toler)
            newList.paths.add(res.paths.toSlice())
            res.paths.count = 0
        }

        .clear()
        .paths = #move newList.paths
        .clean()
    }

    // Clean all paths
    // They must before be flattened
    mtd clean()
    {
        var newList: LinePathList

        foreach p in .paths
        {
            var res = p.polygon.getClean()
            newList.paths.add(res.paths.toSlice())
            res.paths.count = 0
        }

        .clear()
        .paths = #move newList.paths
    }
}
