struct Game
{
    answer: s32
}

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
func play(game: *Game)
{
    //@breakpoint()
    dref game = update(game)
}

//#[Swag.Optim("", false)]
//#[Swag.PrintBc]
func update(game: Game)->Game
{
    @assert(game.answer == 666)
    var result: retval = game
    @assert(game.answer == 666)
    return result
}

#test
{
    var v = Game{}
    v.answer = 666
    //v = update(&v)
    play(&v)
}