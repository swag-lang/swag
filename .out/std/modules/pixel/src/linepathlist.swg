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

    method flatten(quality: PaintQuality)
    {
        visit p: paths
            p.flatten(quality)
    }

    method offset(value: f32)
    {
        visit p: paths
            p.offset(value)
    }

    method offset1(value: f32)
    {
        visit p: paths
            p.offset1(value)
    }

    method cleanPoints()
    {
        visit p: paths
            p.cleanPoints()
    }
}