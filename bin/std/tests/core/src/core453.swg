using Core, Core.Serialization

const FILE = """
<book>
    <author>Gambardella, Matthew</author>
    <title>XML Developer's Guide</title>
    <genre>Computer</genre>
    <price> 44 </price>
    <publish_date>2000-10-01</publish_date>
    <description>An in-depth look at creating applications with XML.</description>
</book>
"""

struct Book
{
    author:           String
    title:            String
    genre:            String
    price:            s32
    publish_date:     String
    description:      String
}

#test
{
    var rea: Decoder'Read.Xml
    let v    = catch rea.readAll'Book(FILE)
    if @err != null:
        Console.print(@err)
    @assert(v.price == 44)
    defer Memory.delete(v)
}
