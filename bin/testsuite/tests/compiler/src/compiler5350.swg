#global testerror "Err0422"
struct String {}
struct(K, V) HashTable {}

func day7A()
{
    var steps: HashTable'(u8, { prev, next: String })

    visit k, v: steps
    {
        visit c: v.prev
        {
            let it = steps.find(c)
            visit it.value.next
            {

            }
        }
    }
}
