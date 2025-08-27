//#global testerror "Err0093"

//#global skip
interface Printable {
    mtd printDetails();
}

struct Book {
    title: string = "TOTO"
}

impl Printable for Book {
    mtd impl printDetails() {
        @print("Book Title: ", title)
    }
}

struct Magazine {
    issueNumber: s32
}

impl Printable for Magazine {
    mtd impl printDetails() {
        @print("Magazine Issue: ", me.issueNumber)
    }
}

//const items: [] Printable = [Book{title: "Swift Programming"}, Magazine{issueNumber: 42}]
const items: Printable = Book{title: "Swift Programming"}

const A: any = Book{title: "Swift Programming"}
//#[Swag.PrintBc]
#test
{
    let ptr = @dataof(A)
    @assert(#typeof(ptr).flags & .Const == .Const)

    //let printableItem = items as Book
    //printableItem.title = "toto"
    //@assert(printableItem.title == "Swift Programming")

    //printableItem.Printable.printDetails()

    /*foreach item in items {
        if let printableItem = item[0] as Book {
            printableItem.Printable.printDetails()
        } elif let printableItem = item as Magazine {
            printableItem.Printable.printDetails()
        }
    }*/
}