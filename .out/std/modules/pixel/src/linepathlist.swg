#global public
using Core, Core.Math

struct LinePathList
{
    paths: Array'(*LinePath)
}

impl LinePathList
{
    method opDrop()
    {
        clear()
    }

    method opPostCopy()
    {
        visit p, i: paths
        {
            ptr := Memory.new'LinePath()
            dref ptr = dref p
            paths[i] = ptr
        }
    }

    #[Swag.inline]
    method count() => paths.count

    // Remove all internal line paths
    method clear()
    {
        visit p: paths
            Memory.delete(p)
        paths.clear()
    }

    // Returns a new path
    method newPath()->*LinePath
    {
        ptr := Memory.new'LinePath()
        paths.add(ptr)
        return ptr
    }

    // Flatten all paths
    method flatten(quality: PaintQuality)
    {
        visit p: paths
            p.flatten(quality)
    }

    // Call fast offset on all paths
    // They must before be flattened
    method offset(value: f32)
    {
        visit p: paths
            p.polygon.offset(value)
    }

    // Call quality offset on all paths
    // They must before be flattened
    method offset(value: f32, joinStyle: JoinStyle)
    {
        visit p: paths
        {
            v := p.polygon.isClockwise ? value : -value
            p.polygon.offset(v, joinStyle)
        }
    }
}