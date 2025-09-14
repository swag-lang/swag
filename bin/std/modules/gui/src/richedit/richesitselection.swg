#global public
using Core

internal impl RichEditCtrl
{
    mtd getSelectionRectRange(lineIndex: u64, rankBeg, rankEnd: *u64)->bool
    {
        let line = me.lines[lineIndex]
        let len  = line.length()

        var x  = me.getMarginLeft()
        var rb = 0'u64
        while x < me.selRectangle.x and rb < len
        {
            x += me.getRuneSize(line, rb, x).x
            rb += 1
        }

        // Line is not concerned
        if x < me.selRectangle.x:
            return false

        var re = rb
        while x < me.selRectangle.right() and re < len
        {
            x += me.getRuneSize(line, re, x).x
            re += 1
        }

        dref rankBeg = rb
        dref rankEnd = re
        return true
    }

    mtd deleteSelectionPrivate(posStart, posEnd: RichEditCursor, selRect: bool)
    {
        let undo = Memory.new'RichEditUndo()
        undo.type = .Delete
        (undo.runes, undo.styles) = me.getTextPrivate(posStart, posEnd)
        undo.selBeg     = me.selBeg
        undo.cursorPos  = me.cursorPos
        undo.hasSelRect = selRect
        undo.serial     = 0
        me.resetUndo()
        me.toUndo.add(undo)
        me.deleteTextPrivate(posStart, posEnd, selRect)
    }

    mtd undoRestoreSelection()
    {
        let undo = Memory.new'RichEditUndo()
        undo.type      = .Selection
        undo.selBeg    = me.selBeg
        undo.cursorPos = me.cursorPos
        undo.serial    = 0
        me.resetUndo()
        me.toUndo.add(undo)
    }

    mtd computeRectangularSelection(beg, end: RichEditCursor)
    {
        if !me.hasSelRect:
            return

        let size = me.getXExtent()

        var ptBeg = me.getCursorDisplayPos(beg)
        var ptEnd = me.getCursorDisplayPos(end)

        if ptBeg.x > ptEnd.x:
            swap(&ptBeg, &ptEnd)

        me.selRectangle.x = ptBeg.x
        me.selRectangle.y = Math.min(ptBeg.y, ptEnd.y) - size.y
        me.selRectangle.setRight(ptEnd.x)
        me.selRectangle.setBottom(Math.max(ptBeg.y, ptEnd.y))
    }
}

impl RichEditCtrl
{
    mtd selectAll()
    {
        me.moveCursorStartFile(&me.selBeg)
        me.moveCursorEndFile(&me.cursorPos)
        me.setRectangularSelection(false)
    }

    mtd setRectangularSelection(selRect: bool)
    {
        me.hasSelRect = selRect
    }

    mtd clearSelection()
    {
        me.selBeg = me.cursorPos
        me.setRectangularSelection(false)
    }

    mtd setSelection(start, end: RichEditCursor)
    {
        me.selBeg    = start
        me.cursorPos = end
        me.checkSelection()
    }

    mtd isSelectionEmpty()->bool
    {
        if me.selBeg.lineIndex != me.cursorPos.lineIndex:
            return false
        if me.selBeg.charPosInLine != me.cursorPos.charPosInLine:
            return false
        return true
    }

    mtd getBegSelection()->RichEditCursor
    {
        if me.hasSelRect
        {
            var result: retval
            result.lineIndex     = Math.min(me.selBeg.lineIndex, me.cursorPos.lineIndex)
            result.charPosInLine = Math.min(me.selBeg.charPosInLine, me.cursorPos.charPosInLine)
            return result
        }

        if me.selBeg.lineIndex < me.cursorPos.lineIndex:
            return me.selBeg
        if me.selBeg.lineIndex > me.cursorPos.lineIndex:
            return me.cursorPos
        if me.selBeg.charPosInLine <= me.cursorPos.charPosInLine:
            return me.selBeg
        return me.cursorPos
    }

    mtd getEndSelection()->RichEditCursor
    {
        if me.hasSelRect
        {
            var result: retval
            result.lineIndex     = Math.max(me.selBeg.lineIndex, me.cursorPos.lineIndex)
            result.charPosInLine = Math.max(me.selBeg.charPosInLine, me.cursorPos.charPosInLine)
            return result
        }

        if me.selBeg.lineIndex < me.cursorPos.lineIndex:
            return me.cursorPos
        if me.selBeg.lineIndex > me.cursorPos.lineIndex:
            return me.selBeg
        if me.selBeg.charPosInLine <= me.cursorPos.charPosInLine:
            return me.cursorPos
        return me.selBeg
    }

    mtd checkSelection()
    {
        if me.selBeg.lineIndex >= me.lines.count:
            me.selBeg.lineIndex = me.lines.count - 1
        if me.cursorPos.lineIndex >= me.lines.count:
            me.cursorPos.lineIndex = me.lines.count - 1

        if me.firstVisibleLineView >= me.lines.count or me.lastVisibleLineView >= me.lines.count
        {
            me.firstVisibleLineView = 0
            me.lastVisibleLineView  = 0
            me.dirtyFirstLineView   = true
            me.invalidate()
        }

        let back = me.lines.back()
        if me.selBeg.lineIndex == me.lines.count - 1 and me.selBeg.charPosInLine > back.length():
            me.selBeg.charPosInLine = back.length()

        if me.cursorPos.lineIndex == me.lines.count - 1 and me.cursorPos.charPosInLine > back.length():
            me.cursorPos.charPosInLine = back.length()

        me.computeRectangularSelection(me.getBegSelection(), me.getEndSelection())
    }

    mtd deleteSelection()
    {
        if me.isReadOnly():
            return
        if me.isSelectionEmpty():
            return

        me.pushUndo()
        let cp = me.getBegSelection()
        me.deleteSelectionPrivate(cp, me.getEndSelection(), me.hasSelRect)
        me.computeScrollY()

        if me.hasSelRect
        {
            me.cursorPos.charPosInLine = Math.min(me.cursorPos.charPosInLine, me.selBeg.charPosInLine)
            me.selBeg.charPosInLine    = me.cursorPos.charPosInLine
        }
        else
        {
            me.setCursorPos(cp)
            me.selBeg = cp
        }

        me.popUndo()
    }

    mtd deleteLeft()
    {
        me.pushUndo()

        if me.isSelectionEmpty()
        {
            me.moveCursorLeft(&me.cursorPos)
        }
        elif me.hasSelRect and me.selBeg.charPosInLine == me.cursorPos.charPosInLine
        {
            var cp = me.cursorPos
            me.moveCursorLeft(&cp)
            if cp.lineIndex == me.cursorPos.lineIndex
            {
                me.cursorPos = cp
                me.computeRectangularSelection(me.getBegSelection(), me.getEndSelection())
            }
        }

        me.deleteSelection()
        me.popUndo()
    }

    mtd deleteRight()
    {
        me.pushUndo()
        if me.isSelectionEmpty() or (me.hasSelRect and me.selBeg.charPosInLine == me.cursorPos.charPosInLine)
        {
            me.moveCursorRight(&me.selBeg)
            me.computeRectangularSelection(me.getBegSelection(), me.getEndSelection())
        }

        me.deleteSelection()
        me.popUndo()
    }

    mtd deleteWordLeft()
    {
        me.pushUndo()
        me.deleteSelection()
        me.moveCursorWordLeft(&me.cursorPos)
        me.deleteSelection()
        me.clearSelection()
        me.popUndo()
    }

    mtd deleteWordRight()
    {
        me.pushUndo()
        me.deleteSelection()
        me.moveCursorWordRight(&me.cursorPos, true)
        me.deleteSelection()
        me.clearSelection()
        me.popUndo()
    }

    internal mtd registerClipboardFormat()
    {
        me.clipFormat = catch Clipboard.registerFormat("std.richedit.style")
    }

    mtd copyToClipboard()
    {
        if me.isSelectionEmpty():
            return
        let res = me.getTextPrivate(me.getBegSelection(), me.getEndSelection())

        me.registerClipboardFormat()
        if me.clipFormat:
            Clipboard.addTypedData(me.clipFormat, null, res.styles.toSlice())

        let txt = Utf8.fromUnicode(res.text.toSlice())
        Clipboard.addString(txt)
    }

    mtd pasteFromClipboard()
    {
        let txt = Clipboard.getString()
        me.registerClipboardFormat()

        me.pushUndo()
        me.deleteSelection()

        if me.clipFormat and Clipboard.hasFormat(me.clipFormat)
        {
            let st = Clipboard.getTypedData(me.clipFormat, null)
            me.insertText(txt, st)
        }
        else:
            me.insertText(txt)

        me.popUndo()
        me.forceCaretVisible = true
    }

    #[Swag.Overload]
    mtd deleteLine()
    {
        me.pushUndo()
        var beg = me.getBegSelection()
        var end = me.getEndSelection()
        if end.charPosInLine == 0 and beg.lineIndex != end.lineIndex:
            me.moveCursorLeft(&end)
        beg.charPosInLine = 0
        end.charPosInLine = me.lines[end.lineIndex].length()
        me.selBeg         = beg
        me.setCursorPos(end)
        me.moveCursorRight(&me.cursorPos)
        me.copyToClipboard()
        me.deleteSelection()
        me.popUndo()
    }

    mtd selectWord()
    {
        let line = me.lines[me.cursorPos.lineIndex]
        let len  = line.length()
        if !len:
            return

        if me.cursorPos.charPosInLine == len:
            me.cursorPos.charPosInLine -= 1
        let cp = me.cursorPos.charPosInLine ? line.chars[me.selBeg.charPosInLine - 1] : 0'rune
        let c  = me.selBeg.charPosInLine == len ? 0'rune : line.chars[me.selBeg.charPosInLine]
        let cn = me.cursorPos.charPosInLine != len - 1 ? line.chars[me.selBeg.charPosInLine + 1] : 0'rune

        if Unicode.isSpace(c) and !Unicode.isSpace(cp) and !Unicode.isSpace(cn) and me.cursorPos.charPosInLine:
            me.cursorPos.charPosInLine -= 1

        me.selBeg = me.cursorPos

        // Blanks
        if Unicode.isSpace(line.chars[me.selBeg.charPosInLine])
        {
            while me.selBeg.charPosInLine and Unicode.isSpace(line.chars[me.selBeg.charPosInLine - 1]):
                me.selBeg.charPosInLine -= 1
            while me.cursorPos.charPosInLine < len and Unicode.isSpace(line.chars[me.cursorPos.charPosInLine]):
                me.cursorPos.charPosInLine += 1
            return
        }

        // A word
        if isWord(line.chars[me.cursorPos.charPosInLine])
        {
            while me.selBeg.charPosInLine and isWord(line.chars[me.selBeg.charPosInLine - 1]):
                me.selBeg.charPosInLine -= 1
            while (me.cursorPos.charPosInLine < len and isWord(line.chars[me.cursorPos.charPosInLine])):
                me.cursorPos.charPosInLine += 1
            return
        }

        // Nothing
        if !zapWordChar(line.chars[me.selBeg.charPosInLine])
        {
            if me.selBeg.charPosInLine == len:
                me.selBeg.charPosInLine -= 1
            else:
                me.cursorPos.charPosInLine += 1
            return
        }

        while me.selBeg.charPosInLine and zapWordChar(line.chars[me.selBeg.charPosInLine - 1]):
            me.selBeg.charPosInLine -= 1
        while me.cursorPos.charPosInLine < len and zapWordChar(line.chars[me.cursorPos.charPosInLine]):
            me.cursorPos.charPosInLine += 1
    }

    mtd scrollLineUp()
    {
        let pt   = me.scrollWnd.getScrollPos()
        let size = me.getXExtent()

        var sy = (pt.y / size.y) * size.y
        sy -= size.y

        me.scrollWnd.setScrollPos(pt.x, sy)
        sy = me.scrollWnd.getScrollPos().y

        let clientRect = me.edView.getClientRect()
        var pos        = me.getCaretPos(me.cursorPos)
        while pos.bottom() > sy + clientRect.height
        {
            if !me.moveCursorUp(&me.cursorPos):
                break
            me.clearSelection()
            pos = me.getCaretPos(me.cursorPos)
        }
    }

    mtd scrollLineDown()
    {
        let pt   = me.scrollWnd.getScrollPos()
        let size = me.getXExtent()

        var sy = (pt.y / size.y) * size.y
        sy += size.y

        me.scrollWnd.setScrollPos(pt.x, sy)
        sy = me.scrollWnd.getScrollPos().y

        var pos = me.getCaretPos(me.cursorPos)
        while pos.y < sy
        {
            if !me.moveCursorDown(&me.cursorPos):
                break
            me.clearSelection()
            pos = me.getCaretPos(me.cursorPos)
        }
    }
}
