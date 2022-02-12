#global public
using Core, Pixel

struct Item
{
    name:       RichString
    icon:       Icon
    id:         WndId
    userData:   *void
}
