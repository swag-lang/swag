#global testerror "Err0669"
struct String {}
struct(K, V) HashTable {}

func day7A()
{
    var steps: HashTable'(u8, { prev, next: String })

    foreach k, v in steps
    {
        foreach c in v.prev
        {
            let it = steps.find(c)
            foreach it.value.next
            {

            }
        }
    }
}
