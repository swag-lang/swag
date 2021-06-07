#global public
using Core, Core.Math

struct LinePathList
{
    paths: Array'(*LinePath)
}

impl LinePathList
{
    func opDrop(using self)
    {
        clear()
    }

    // Remove all internal line paths
    func clear(using self)
    {
        visit p: paths
            Memory.delete(p)
        paths.clear()
    }

    // Returns a new path
    func newPath(using self)->*LinePath
    {
        ptr := Memory.new'LinePath()
        paths.add(ptr)
        return ptr
    }
}