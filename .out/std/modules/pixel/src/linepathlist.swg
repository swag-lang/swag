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
}