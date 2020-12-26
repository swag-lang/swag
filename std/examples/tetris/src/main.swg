/*
	OneLoneCoder.com - Command Line Tetris
	"Put Your Money Where Your Mouth Is" - @Javidx9

	License
	~~~~~~~
	Copyright (C) 2018  Javidx9
	This program comes with ABSOLUTELY NO WARRANTY.
	This is free software, and you are welcome to redistribute it
	under certain conditions; See license for details.
	Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9
	GNU GPLv3
	https://github.com/OneLoneCoder/videos/blob/master/LICENSE
	From Javidx9 :)
	~~~~~~~~~~~~~~~
	Hello! Ultimately I don't care what you use this for. It's intended to be
	educational, and perhaps to the oddly minded - a little bit of fun.
	Please hack this, change it and use it in any way you see fit. You acknowledge
	that I am not responsible for anything bad that happens as a result of
	your actions. However this code is protected by GNU GPLv3, see the license in the
	github repo. This means you must attribute me if you use it. You can view this
	license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
	Cheers!

	Background
	~~~~~~~~~~
	I made a video "8-Bits of advice for new programmers" (https://youtu.be/vVRCJ52g5m4)
	and suggested that building a tetris clone instead of Dark Sould IV might be a better
	approach to learning to code. Tetris is nice as it makes you think about algorithms.

	Controls are Arrow keys Left, Right & Down. Use Z to rotate the piece.
	You score 25pts per tetronimo, and 2^(number of lines)*100 when you get lines.

	Future Modifications
	~~~~~~~~~~~~~~~~~~~~
	1) Show next block and line counter

	Author
	~~~~~~
	Twitter: @javidx9
	Blog: www.onelonecoder.com

	Video:
	~~~~~~
	https://youtu.be/8OK8_tHeCIA

	Last Updated: 30/03/2017
*/
#if #os == "windows";
#import "core"
using core, win32

var nScreenWidth = 120'u32			// Console Screen Size X (columns)
var nScreenHeight = 30'u32			// Console Screen Size Y (rows)
var nFieldWidth = 12
var nFieldHeight = 18
var pField: *u8
var tetromino: [7] string

func Rotate(px, py, r: s32)->s32
{
	pi := 0
	switch r % 4
	{
	case 0: pi = py * 4 + px
	case 1: pi = 12 + py - (px * 4)
	case 2: pi = 15 - (py * 4) - px
	case 3: pi = 3 - py + (px * 4)
    }

	return pi;
}

func DoesPieceFit(nTetromino, nRotation, nPosX, nPosY: s32)->bool
{
	// All Field cells >0 are occupied
    loop px: 4
    {
		loop py: 4
		{
			// Get index into piece
			pi := Rotate(cast(s32) px, cast(s32) py, nRotation)

			// Get index into field
			fi := (nPosY + py) * nFieldWidth + (nPosX + px)

			// Check that test is in bounds. Note out of bounds does
			// not necessarily mean a fail, as the long vertical piece
			// can have cells that lie outside the boundary, so we'll
			// just ignore them
			if nPosX + px >= 0 && nPosX + px < nFieldWidth
			{
				if nPosY + py >= 0 && nPosY + py < nFieldHeight
				{
					// In Bounds so do collision check
                    str := tetromino[nTetromino]
					if str[pi] != "."'u8 && pField[fi] != 0
						return false; // fail on first hit
				}
			}
		}
    }

	return true
}

//#[swag.printbc]
func main()
{
    var screen: *u8 = @alloc(cast(uint) nScreenWidth*nScreenHeight)
    loop i: nScreenWidth*nScreenHeight
        screen[i] = " "'u8
    hConsole := CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, null, CONSOLE_TEXTMODE_BUFFER, null)
	SetConsoleActiveScreenBuffer(hConsole)
	var dwBytesWritten: DWORD = 0

	tetromino[0] = "..X...X...X...X."
	tetromino[1] = "..X..XX...X....."
	tetromino[2] = ".....XX..XX....."
	tetromino[3] = "..X..XX..X......"
	tetromino[4] = ".X...XX...X....."
	tetromino[5] = ".X...X...XX....."
	tetromino[6] = "..X...X..XX....."

    pField = @alloc(cast(uint)(nFieldWidth*nFieldHeight)) // Create play field buffer
    loop x: autocast nFieldWidth
        loop y: autocast nFieldHeight
			pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

    nCurrentPiece := 0
    nCurrentRotation := 0
    nCurrentX := nFieldWidth / 2
	nCurrentY := 0
    nSpeed := 20
	nSpeedCount := 0
    nPieceCount := 0
    bGameOver := false
    bForceDown := false
    nScore := 0

    var vLines: Array's32
    var rand: random.Rng
    var builder: strconv.StringBuilder
    var keyb: keyboard.Keyboard

    while !bGameOver
    {
		// Timing =======================
        thread.sleep(50)
        nSpeedCount += 1
		bForceDown = (nSpeedCount == nSpeed)

        // Input ========================
        using keyboard.Key
        keyb.update()
        if keyb.isKeyPressed(Escape)
            break

        // Game Logic ===================

        // Handle player movement
		nCurrentX += (keyb.isKeyPressed(Right) && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0
		nCurrentX -= (keyb.isKeyPressed(Left) && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0
		nCurrentY += (keyb.isKeyPressed(Down) && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0
        nCurrentRotation += (keyb.isKeyJustPressed(Space) && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0

		// Force the piece down the playfield if it's time
		if (bForceDown)
		{
            nSpeedCount = 0
			nPieceCount += 1
			if nPieceCount % 50 == 0 && nSpeed >= 10
				nSpeed -= 1

			// Test if piece can be moved down
			if DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)
				nCurrentY += 1 // It can, so do it!
            else
            {
				// It can't! Lock the piece in place
				for px := 0; px < 4; px += 1
                {
					for py := 0; py < 4; py += 1
                    {
                        str := tetromino[nCurrentPiece]
						if str[Rotate(px, py, nCurrentRotation)] != "."'u8
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = autocast (nCurrentPiece + 1)
                    }
                }

				// Check for lines
				for py := 0; py < 4; py += 1
                {
					if nCurrentY + py < nFieldHeight - 1
					{
						bLine := true
						for px := 1; px < nFieldWidth - 1; px += 1
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0

						if (bLine)
						{
							// Remove Line, set to =
							for px := 1; px < nFieldWidth - 1; px += 1
								pField[(nCurrentY + py) * nFieldWidth + px] = 8
							vLines.add(nCurrentY + py)
						}
					}
                }

                nScore += 25
                if !vLines.isEmpty() nScore += (1 << cast(u32) vLines.count) * 100

				// Pick New Piece
				nCurrentX = nFieldWidth / 2
				nCurrentY = 0;
				nCurrentRotation = 0
				nCurrentPiece = autocast (rand.nextU32() % 7)

                // If piece does not fit straight away, game over!
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY)
            }
        }

        // Display ======================

        // Draw Field
        const charD = " ABCDEFG=#"
        for x := 0; x < nFieldWidth; x += 1
        {
			for y := 0; y < nFieldHeight; y += 1
            {
				screen[(y + 2)*nScreenWidth + (x + 2)] = charD[pField[y*nFieldWidth + x]]
            }
        }

        // Draw Current Piece
		for px := 0; px < 4; px += 1
        {
			for py := 0; py < 4; py += 1
            {
                str := tetromino[nCurrentPiece]
				if str[Rotate(cast(s32) px, cast(s32) py, nCurrentRotation)] != "."'u8
					screen[(nCurrentY + py + 2)*nScreenWidth + (nCurrentX + px + 2)] = autocast (nCurrentPiece + 65)
            }
        }

        // Draw Score
        builder.clear()
        builder.appendFormat("SCORE: %{D08}", nScore)
        strScore := builder.moveToString()
        memory.copy(&screen[2 * nScreenWidth + nFieldWidth + 6], strScore.buffer, cast(uint) strScore.length)

		// Animate Line Completion
		if !vLines.isEmpty()
		{
			// Display Frame (cheekily to draw lines)
			WriteConsoleOutputCharacterA(hConsole, screen, nScreenWidth * nScreenHeight, 0, &dwBytesWritten);
			thread.sleep(400)

			visit v: vLines
            {
				for px := 1; px < nFieldWidth - 1; px += 1
				{
					for py := v; py > 0; py -= 1
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px]
					pField[px] = 0
				}
            }

			vLines.clear();
		}

        // Display Frame
		res := WriteConsoleOutputCharacterA(hConsole, screen, nScreenWidth * nScreenHeight, 0, &dwBytesWritten)
    }

    CloseHandle(hConsole)
}

//#run main()
#main
{
    main()
}