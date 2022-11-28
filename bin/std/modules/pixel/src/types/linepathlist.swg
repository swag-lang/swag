#global public
using Core, Core.Math

struct LinePathList
{
    paths:  Array'(*LinePath)
    bvMin:  Vector2
    bvMax:  Vector2
}

impl LinePathList
{
    mtd opDrop()
    {
        clear()
    }

    mtd opPostCopy()
    {
        visit p, i: paths
        {
            ptr := Memory.new'LinePath()
            dref ptr = dref p
            paths[i] = ptr
        }
    }

    #[Swag.Inline]
    mtd count() => paths.count

    // Remove all internal line paths
    mtd clear()
    {
        visit p: paths
            Memory.delete(p)
        paths.clear()
    }

    // Returns a new path
    mtd newPath()->*LinePath
    {
        ptr := Memory.new'LinePath()
        paths.add(ptr)
        return ptr
    }

    // Flatten all paths
    mtd flatten(quality: PaintQuality)
    {
        // Flatten all path, and compute BV
        bvMin = Vector2.Max
        bvMax = Vector2.NegMax
        visit p: paths
        {
            p.flatten(quality)
            bvMin = Vector2.min(bvMin, p.bvMin)
            bvMax = Vector2.max(bvMax, p.bvMax)
        }
    }

    // Call fast offset on all paths
    // They must before be flattened
    mtd offset(value: f32)
    {
        if Math.isZeroEpsilon(value)
            return

        visit p: paths
        {
            v := p.polygon.isClockwise ? value : -value
            p.polygon.offset(v)
        }
    }

    // Call quality offset on all paths
    // They must before be flattened
    mtd offset(value: f32, joinStyle: JoinStyle, toler: f32 = 0.5)
    {
        if Math.isZeroEpsilon(value)
            return

        var newList: LinePathList

        visit p: paths
        {
            v := p.polygon.isClockwise ? value : -value

            res := p.polygon.getOffset(v, joinStyle, toler)
            loop i: res.paths
            {
                pl := res.paths[i]
                newList.paths.add(pl)
            }

            res.paths.count = 0
        }

        paths =,move newList.paths
        clean()
    }

    // Clean all paths
    // They must before be flattened
    mtd clean()
    {
        var newList: LinePathList

        visit p: paths
        {
            res := p.polygon.getClean()
            loop i: res.paths
            {
                pl := res.paths[i]
                newList.paths.add(pl)
            }

            res.paths.count = 0
        }

        paths =,move newList.paths
    }
}