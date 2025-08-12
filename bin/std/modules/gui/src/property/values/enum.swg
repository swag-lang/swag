impl Properties
{
    func refreshEnumFlags(item: *PropertiesItem)
    {
        if item.checkBox
        {
            var val1, val2: u64
            switch item.typeValue.pointedType.sizeof
            {
            case 1:
                val1 = cast(u64) dref cast(*u8) item.data
                val2 = cast(u64) dref cast(const *u8) item.typeValue.value
            case 2:
                val1 = cast(u64) dref cast(*u16) item.data
                val2 = cast(u64) dref cast(const *u16) item.typeValue.value
            case 4:
                val1 = cast(u64) dref cast(*u32) item.data
                val2 = cast(u64) dref cast(const *u32) item.typeValue.value
            case 8:
                val1 = cast(u64) dref cast(*u64) item.data
                val2 = cast(u64) dref cast(const *u64) item.typeValue.value
            }

            item.checkBox.checked = val1 & val2 ? .Checked : .UnChecked
            item.checkBox.invalidate()
        }
    }

    func refreshEnum(item: *PropertiesItem)
    {
        if item.comboBox
        {
            let typeEnum = cast(const *Swag.TypeInfoEnum) item.typeValue.pointedType

            var val1, val2: u64
            var val1s, val2s: string
            switch typeEnum.rawType.sizeof
            {
            case 1:
                val1 = cast(u64) dref cast(*u8) item.data
            case 2:
                val1 = cast(u64) dref cast(*u16) item.data
            case 4:
                val1 = cast(u64) dref cast(*u32) item.data
            case 8:
                val1 = cast(u64) dref cast(*u64) item.data
            case 16:
                val1s = dref cast(*string) item.data
            }

            var idxSel = 0'u64
            foreach v, idx in typeEnum.values
            {
                switch typeEnum.rawType.sizeof
                {
                case 1:
                    val2 = cast(u64) dref cast(const *u8) v.value
                case 2:
                    val2 = cast(u64) dref cast(const *u16) v.value
                case 4:
                    val2 = cast(u64) dref cast(const *u32) v.value
                case 8:
                    val2 = cast(u64) dref cast(const *u64) v.value
                case 16:
                    val2s = dref cast(const *string) v.value
                }

                if val1 == val2 and val1s == val2s:
                    idxSel = idx
            }

            item.comboBox.selectItem(cast() idxSel)
        }
    }

    mtd validateEnumFlags(item: *PropertiesItem)
    {
        if item.checkBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                let p  = cast(*T) item.data
                let p1 = cast(*T) item.checkBox.userData
                if item.checkBox.checked == .Checked:
                    dref p |= dref p1
                else:
                    dref p &= ~(dref p1)
            }

            switch item.typeValue.pointedType.sizeof
            {
            case 1:
                setEd'u8()
            case 2:
                setEd'u16()
            case 4:
                setEd'u32()
            case 8:
                setEd'u64()
            }
        }

        hasChanged(item)
    }

    mtd validateEnum(item: *PropertiesItem)
    {
        if item.comboBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                let p  = cast(*T) item.data
                let p1 = cast(const *T) typeEnum.values[sel].value
                dref p = dref p1
            }

            let typeEnum = cast(const *Swag.TypeInfoEnum) item.typeValue.pointedType
            let sel      = item.comboBox.selectedIdx
            switch typeEnum.rawType.sizeof
            {
            case 1:
                setEd'u8()
            case 2:
                setEd'u16()
            case 4:
                setEd'u32()
            case 8:
                setEd'u64()
            case 16:
                setEd'string()
            }
        }

        hasChanged(item)
    }

    mtd constructEnumFlags(value: Swag.TypedValue, typeEnum: const *Swag.TypeInfoEnum, data: *u8, parentItem: *PropertiesItem)
    {
        let item = addItem(value, data, parentItem)
        item.flags.add(.NoSeparator)

        foreach v in typeEnum.values
        {
            with let line = Wnd.create'Wnd(edView)
            {
                .wndFlags.set(.PreChildsPaint)
                if kind == .List:
                    .margin.x = leftMargin
                .margin.z = rightMargin
                .margin.y = listNameMarginAfter
                .margin.w = listNameMarginAfter
            }

            with let itemB = items.addNewPtr()
            {
                .type        = typeEnum
                .typeValue   = v
                .data        = data
                .extendFocus = 5
                .filter      = item.filter
                .line        = line
                .parent      = item
                .level       = item.level + 1
                .section     = item.section
                .flags.add(.NoSeparator)
            }

            line.hookOnEvent = func|using self, itemB|(hwnd, evt) { return onItemEvent(itemB, hwnd, evt); }

            with let ed = CheckButton.create(line, "", {0, 0, gridLineHeight, 0})
            {
                if kind == .Grid:
                    .margin.x = leftMargin
                .dockStyle = .Left
                .userData  = cast #unconst (*void) v.value
            }

            itemB.checkBox = ed

            let dispDesc = Reflection.getAttributeValue(v, Properties.Description, "desc")
            itemB.filter += cast(string) dispDesc

            with let label1 = Label.create(line, "", {item.line.position.height, 0, position.width - item.line.position.height})
            {
                .dockStyle = .Center
                .labelFlags |= .WordWrap | .AutoHeight
                .horzAlignmentText = .Left
                .setText(dispDesc)
            }

            let minHeight = ed.position.height
            itemB.sigResize += func|label1, line, minHeight|(itemB)
            {
                label1.autoHeight()
                line.position.height = Math.max(minHeight, label1.position.height)
                line.applyLayout()
            }

            refreshEnumFlags(itemB)
            itemB.checkBox.sigChanged += func|using self, itemB|(ed)
            {
                validateEnumFlags(itemB)
            }
        }
    }

    mtd constructEnum(value: Swag.TypedValue, typeEnum: const *Swag.TypeInfoEnum, data: *u8, parentItem: *PropertiesItem)
    {
        if Reflection.hasAttribute(typeEnum, Swag.EnumFlags)
        {
            constructEnumFlags(value, typeEnum, data, parentItem)
            return
        }

        let item = addItem(value, data, parentItem)
        with let ed = ComboBox.create(item.line, kind: .CheckBox)
        {
            .maxSize.x = cast() Reflection.getAttributeValue(value, EditSize, "size", sizeEditValue)
            if kind == .Grid:
                .dockStyle = .CenterKeepSizeVert
            else:
                .anchorStyle = .X
        }

        item.comboBox = ed

        foreach v, idx in typeEnum.values:
            ed.addItem(v.name)

        refreshEnum(item)
        item.comboBox.sigChanged += func|using self, item|(ed, n)
        {
            validateEnum(item)
        }
    }
}
