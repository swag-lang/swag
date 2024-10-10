//#global testerror "Err0093"

var G = 0

//#global skip
interface Printable {
    mtd const printDetails();
}

struct Book {
    title: string = "TOTO"
}

impl Printable for Book {       
    mtd const impl printDetails() {
        if title == "Swift Programming":
            G += 1
    }
}

struct Magazine {
    issueNumber: s32
}
    
impl Printable for Magazine {       
    mtd const impl printDetails() {
        G += issueNumber
    }
}

const items: [] Printable = [Book{title: "Swift Programming"}, Magazine{issueNumber: 42}]

//#[Swag.PrintBc]
#test
{   
    foreach item in items {
        if let printableItem = item as Book {
            printableItem.Printable.printDetails()
        } elif let printableItem = item as Magazine {
            printableItem.Printable.printDetails()
        }
    }

    @assert(G == 43)
}