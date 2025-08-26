#global marked
#global public
using Core, Pixel

struct Item
{
    name:          RichString
    icon:          Icon
    id:            WndId
    userData0:     *void
    userData1:     *void
}
