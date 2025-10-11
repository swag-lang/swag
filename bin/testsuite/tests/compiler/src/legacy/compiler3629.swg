struct A {}
struct B { using a: A }

alias Alias0 = *void
alias Alias1 = *void

impl A
{
    #[Swag.Inline, Swag.Overload]
    func(T) create(parent: #null *void, position: f32, id: Alias0 = null, hook: Alias1 = null)
    {
        create(parent, "", position, hook: hook)
    }

    #[Swag.Inline, Swag.Overload]
    func(T) create(parent: #null *void, name: string, position: f32, id: Alias0 = null, hook: Alias1 = null) {}
}

impl B
{
    func create(parent: #null *void, name: string, position: f32, id: Alias0 = null)
    {
        A.create'B(parent, position, id)
    }
}

#test
{
    B.create(null, "", 0)
}
