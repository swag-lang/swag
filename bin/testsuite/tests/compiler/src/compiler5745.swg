//#global testerror "Err0090"
var G: string

//#global skip
interface Printable {
    mtd const printDetails();
}

struct Book {
    title: string = "TOTO"
}

impl Printable for Book {       
    mtd const impl printDetails() {
        G = title
    }
}

struct Magazine {
    issueNumber: s32
}
    
impl Printable for Magazine {       
    mtd const impl printDetails() {
        @print("Magazine Issue: ", issueNumber)
    }
}

//const items: [] Printable = [Book{title: "Swift Programming"}, Magazine{issueNumber: 42}]
const items: Printable = Book{title: "Swift Programming"}

//#[Swag.PrintBc]
#test
{   
    let printableItem = items as Book
    printableItem.Printable.printDetails()
    @assert(G == "Swift Programming")

    /*foreach item in items {
        if let printableItem = item[0] as Book {
            printableItem.Printable.printDetails()
        } elif let printableItem = item as Magazine {
            printableItem.Printable.printDetails()
        }
    }*/
}