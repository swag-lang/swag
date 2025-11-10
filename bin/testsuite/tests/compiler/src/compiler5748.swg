//#global testerror "Err0017"

var G = 0

//#global skip
interface Printable
{
    mtd const printDetails();
}

struct Book
{
    title: string = "TOTO"
}

impl Printable for Book
{
    mtd const impl printDetails()
    {
        if me.title == "Swift Programming" do
            G += 1
    }
}

struct Magazine
{
    issueNumber: s32
}

impl Printable for Magazine
{
    mtd const impl printDetails()
    {
        G += me.issueNumber
    }
}

//#[Swag.PrintBc]
#test
{
    var items: [?] Printable = [Book{title: "Swift Programming"}, Magazine{issueNumber: 42}]

    foreach item in items
    {
        item.printDetails()
    }

    @assert(G == 43)
}
