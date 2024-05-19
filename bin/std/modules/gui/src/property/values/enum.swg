impl PropertyList
{
    func refreshEnumFlags(item: *PropertyListItem)
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

    func refreshEnum(item: *PropertyListItem)
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
            visit v, idx: typeEnum.values
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

                if val1 == val2 and val1s == val2s do
                    idxSel = idx
            }

            item.comboBox.selectItem(acast idxSel)
        }
    }

    mtd validateEnumFlags(item: *PropertyListItem)
    {
        if item.checkBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                let p  = cast(*T) item.data
                let p1 = cast(*T) item.checkBox.userData
                if item.checkBox.checked == .Checked do
                    dref p |= dref p1
                else do
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

    mtd validateEnum(item: *PropertyListItem)
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

    mtd constructEnumFlags(value: Swag.TypeValue, typeEnum: const *Swag.TypeInfoEnum, data: *u8, parentItem: *PropertyListItem)
    {
        let item = addItem(value, data, parentItem, listNameHeight)
        item.flags.add(.NoSeparator)

        visit v: typeEnum.values
        {
            with let line = Wnd.create'Wnd(edView)
            {
                .margin.x = leftMargin
                .margin.z = rightMargin
                .margin.y = listNameMarginAfter
                .margin.w = listNameMarginAfter
                .wndFlags.set(.PreChildsPaint)
            }

            with let itemB = items.addNewPtr()
            {
                .type        = typeEnum
                .typeValue   = v
                .data        = data
                .extendFocus = 5
                .filter      = item.filter
                .line        = line
                .forceShow   = item.line
                .flags.add(.NoSeparator)
            }

            line.hookOnEvent = closure|self, itemB|(wnd, evt) { return self.onItemEvent(itemB, wnd, evt); }

            let lineHeight = getThemeMetrics().btnPush_Height
            let ed         = CheckButton.create(line, "", {0, 0, lineHeight, 0})
            ed.dockStyle   = .Left
            ed.userData    = cast,unconst(*void) v.value
            itemB.checkBox = ed

            let dispDesc = Reflection.getAttributeValue(v, PropertyList.Description, "desc")
            itemB.filter += cast(string) dispDesc

            with let label1 = Label.create(line, "", {lineHeight, 0, position.width - lineHeight})
            {
                .dockStyle = .Center
                .labelFlags |= .WordWrap | .AutoHeight
                .horzAlignmentText = .Left
                .setText(dispDesc)
            }

            let minHeight = ed.position.height
            itemB.sigResize += closure|label1, line, minHeight|(itemB)
            {
                label1.autoHeight()
                line.position.height = Math.max(minHeight, label1.position.height)
                line.applyLayout()
            }

            refreshEnumFlags(itemB)
            itemB.checkBox.sigChanged += closure|self, itemB|(ed)
            {
                self.validateEnumFlags(itemB)
            }
        }
    }

    mtd constructEnum(value: Swag.TypeValue, typeEnum: const *Swag.TypeInfoEnum, data: *u8, parentItem: *PropertyListItem)
    {
        if Reflection.hasAttribute(typeEnum, Swag.EnumFlags)
        {
            constructEnumFlags(value, typeEnum, data, parentItem)
            return
        }

        let item = addItem(value, data, parentItem, getThemeMetrics().comboBox_Height)
        with let ed = ComboBox.create(item.line, kind: .CheckBox)
        {
            .maxSize.x = acast Reflection.getAttributeValue(value, EditSize, "size", sizeEditValue)
            if kind == .Grid do
                .dockStyle = .CenterKeepSizeVert
            else do
                .anchorStyle = .X
        }

        item.comboBox = ed

        visit v, idx: typeEnum.values do
            ed.addItem(v.name)

        refreshEnum(item)
        item.comboBox.sigChanged += closure|self, item|(ed, n)
        {
            self.validateEnum(item)
        }
    }
}
