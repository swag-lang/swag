#global testerror "Err0736"
struct String {}
struct(K, V) HashTable {}

func day7A()
{
    var steps: HashTable'(u8, { prev, next: String })

    visit k, v in steps
    {
        visit c in v.prev
        {
            let it = steps.find(c)
            visit it.value.next
            {

            }
        }
    }
}
