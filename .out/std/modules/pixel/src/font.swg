#global public
using Core, FreeType

struct Font
{
    typeFace:   const *TypeFace
    size:       u32
}

impl Font
{
    // Creates a new Font of the given typeface and size
    func create(typeFace: TypeFace, size: u32)->Font
    {
        var result: retval
        result.typeFace = typeFace
        result.size = size
        return result
    }
}