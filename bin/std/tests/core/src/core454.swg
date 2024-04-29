using Core, Core.Serialization

struct AA
{
    x: s32
}

const FILE = """
<book>
    <author>Gambardella, Matthew</author>
    <title>XML Developer's Guide</title>
    <genre>Computer</genre>
    <price>44</price>
    <publish_date>
        <x>42</x>
    </publish_date>
    <description>An in-depth look at creating applications with XML.</description>
</book>
"""

struct Book
{
    author:         String
    title:          String
    genre:          String
    price:          s32
    publish_date:   AA
    description:    String
}

#test
{
    var rea: Decoder'Read.Xml
    let v    = catch rea.readAll'Book(FILE)
    if @err() != null do Console.print(@err())
    @assert(v.price == 44)
    @assert(v.publish_date.x == 42)
    defer Memory.delete(v)
}
