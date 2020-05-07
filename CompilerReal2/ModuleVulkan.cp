EnumsValue := 1
EnumsBitPos := 2

IsReqList := !(TreeNode^ nd, char^ str) -> bool
{
	for c : str, val : nd.Childs
	{
		if c == '0' and val.first return false
		if c == '1' and not val.first return false
	}
	return true
}

IsStrPointer := !( char^ str) -> bool
{
	if str^ != '*' return false
	i := 1
	while str[i] != 0
	{
		if str[i] != ' ' return false
		i++
	}
	return str[i] == 0
}

ModuleVulkan := class extend CompilerModule
{
	xmlTree := TreeNode^
	Enums := AVLMap.{string,Pair.{int,string}}
	Typedefs := AVLMap.{string,Type^}
	ItInts := AVLMap.{string,int}

	Commands := AVLMap.{string,TreeNode^}

	Structs := AVLMap.{string,TreeNode^}

	ParsCount := GlobalParam^
	ParsNames := GlobalParam^


	InstClass := BoxClass^

	this := !(string p) -> void
	{
		xmlFile := MappedFile(p)
		xmlTree = ParseXML(xmlFile.point)

	}
	InitModule := virtual !() -> bool
	{
		InstClass = new InstanceArr(this&)

		Typedefs["float"] = GTypeFloat
		Typedefs["char"] = GetType("char")
		Typedefs["void"] = GTypeVoid
		Typedefs["void*"] = GTypeVoid.GetPoint()
		Typedefs["size_t"] = GetType("u64")

		Typedefs["uint8_t"] = GetType("u8")
		Typedefs["uint16_t"] = GetType("u16")
		Typedefs["uint32_t"] = GetType("u32")
		Typedefs["uint64_t"] = GetType("u64")

		Typedefs["int8_t"] = GetType("s8")
		Typedefs["int16_t"] = GetType("s16")
		Typedefs["int32_t"] = GetType("s32")
		Typedefs["int64_t"] = GetType("s64")
		Typedefs["VkFuncsHolder"] = InstClass.ClassType

		ParsCount = new GlobalParam(GTypeInt,null)
		ParsNames = new GlobalParam(GTypeString.GetPoint(),null)

		for xmlTree.Childs
		{
			if it.first
			{
				asNode := it.second->{TreeNode^}
				switch(asNode.NodeName)
				{
				case "extensions"
					CheckExtensions(asNode)
				}
			}
		}

		for xmlTree.Childs
		{
			if it.first
			{
				asNode := it.second->{TreeNode^}
				switch(asNode.NodeName)
				{
				case "enums"
					CheckEnum(asNode)
				case "types"
					CheckTypes(asNode)
				case "commands"
					CheckCommands(asNode)
				}
			}else{
			}
		}
		return true
	}
	CheckExtensions := !(TreeNode^ nd) -> void
	{
		for sN : nd.Childs
		{
			if not sN.first continue

			asNeed := sN.second->{TreeNode^}
			extNumberPre := asNeed.Attrs.TryFind("number")
			if extNumberPre == null continue
			extNumber := StrToInt(extNumberPre^)
			extNumber -= 1
			extNumber *= 1000

			for sub : asNeed.Childs
			{
				if not sub.first continue
				asN2 := sub.second->{TreeNode^}
				if asN2.NodeName != "require" coninue
				for itms : asN2.Childs
				{
					if not itms.first continue

					asN2 := itms.second->{TreeNode^}
					switch asN2.NodeName
					{
						case "enum"
							itName := asN2.Attrs.TryFind("name")
							if itName != null
							{
								itDir := asN2.Attrs.TryFind("dir")
								if itDir != null assert(itDir^ == "-")
								itOffst := asN2.Attrs.TryFind("offset")
								if itOffst != null
								{
									offst := StrToInt(itOffst^)
									offst += extNumber
									offst += 1000000000
									if itDir != null offst = -offst
									ItInts[itName^] = offst
								}else{
								}
							}
						case void
					}
				}
			}
		}
	}
	CheckCommands := !(TreeNode^ nd) -> void
	{
		for sN : nd.Childs
		{
			if not sN.first continue

			asNeed := sN.second->{TreeNode^}

			if asNeed.NodeName != "command" continue

			pr := asNeed.GetSubNode("proto")
			if pr == null continue
			itName := pr.GetValueString("name")

			Commands[itName] = asNeed
		}
	}
	CheckTypes := !(TreeNode^ nd) -> void
	{

		for typ : nd.Childs //structs only
		if typ.first
		{
			asNeed := typ.second->{TreeNode^}
			category := asNeed.Attrs.TryFind("category")
			if category != null and (category^ == "struct" or category^ == "union")
			{
				stName := asNeed.Attrs.TryFind("name")
				if stName != null{
					Structs[stName^] = asNeed
				}
			}
		}

		for typ : nd.Childs
		if typ.first
		{
			asNeed := typ.second->{TreeNode^}
			isAlias := asNeed.Attrs.TryFind("alias")
			if isAlias != null
			{
				inDefs := Typedefs.TryFind(isAlias^)
				if inDefs != null{
					itName := asNeed.Attrs.TryFind("name")
					if itName != null
					{
						Typedefs[itName^] = inDefs^
					}
				}
			}else{
				category := asNeed.Attrs.TryFind("category")
				if category != null{
					if category^ == "basetype" or category^ == "bitmask"
					{
						if  asNeed.Childs.Size() != 0 and (not asNeed.Childs[0].first) and asNeed.Childs[0].second->{string} == "typedef "
						{
							typeStr := asNeed.GetValueString("type")
							typeName := asNeed.GetValueString("name")
							if typeStr != null and typeName != null{
								inDefs := Typedefs.TryFind(typeStr)
								if inDefs != null{
									Typedefs[typeName] = inDefs^
								}
							}

						}
					}
					if category^ == "handle"
					{
						typeName := asNeed.GetValueString("name")
						if typeName != null
						{
							Typedefs[typeName] = GTypeVoid.GetPoint()
						}
					}
					if category^ == "enum"
					{
						typeName := asNeed.Attrs.TryFind("name")
						if typeName != null{
							Typedefs[typeName^] = GTypeInt //????
						}
					}
					if category^ == "funcpointer"{
						retType := null->{Type^}
						funcTypes := Queue.{Type^}() ; $temp

						if not asNeed.Childs[0].first{
							itRet := asNeed.Childs[0].second->{string}
							switch itRet
							{
								case "typedef void (VKAPI_PTR *"
									retType = GTypeVoid
								case "typedef void* (VKAPI_PTR *"
									retType = GTypeVoid.GetPoint()
								case "typedef VkBool32 (VKAPI_PTR *"
									retType = Typedefs["VkBool32"]
								case void
									printf("VKMODULE: cant read type %s\n",itRet)
									return void
							}
						}
						for pChild : asNeed.Childs, i : 0
						{
							if pChild.first {
								ca := pChild.second->{TreeNode^}
								if ca.NodeName == "type"
								{
									itTypName := ca.Childs[0].second->{string}
									asT := GetModuleType(itTypName)
									if asT == null {
										printf("VKMODULE: cant read type %s byType %s\n",asNeed.GetValueString("name"),itTypName)
										return void
									}
									if  asNeed.Childs[i+1].second->{string}[0] == '*'
									{
										asT = asT.GetPoint()
									}
									funcTypes.Push(asT)
								}
							}
						}
						funcT := GetFuncType(funcTypes,null->{bool^},retType,false,false)
						if funcT != null
							Typedefs[asNeed.GetValueString("name")] = funcT.GetPoint()

					}
				}else{
				}
			}
		}
	}
	CheckEnum := !(TreeNode^ nd) -> void{
		itType := nd.Attrs.TryFind("type")
		if itType == null 
		{
			itName := nd.Attrs.TryFind("name")
			if itName == null or itName^ != "API Constants"
				return void
		}

		for nd.Childs
		{
			if it.first{
				asNode := it.second->{TreeNode^}
				itName := asNode.Attrs.TryFind("name")
				itValue := asNode.Attrs.TryFind("value")

				if itName != null{
					if itValue != null
					{
						inTree := ref Enums[itName^]
						inTree.first = EnumsValue
						inTree.second = itValue^
					}else{
						itBitpos := asNode.Attrs.TryFind("bitpos")
						if itBitpos != null{
							inTree := ref Enums[itName^]
							inTree.first = EnumsBitPos
							inTree.second = itBitpos^
						}
					}
				}
			}
		}		
	}

	GetFromInts := !(string name,int^ res) -> bool
	{
		inInts := ItInts.TryFind(name)
		if inInts == null {
			inEns := Enums.TryFind(name)
			if inEns != null {
				if not TryReadInt(inEns.second,res)
					return false
				if inEns.first == EnumsBitPos res^ = 1 << res^
				ItInts[name] = res^
				return true
			}
			return false
		}
		res^ = inInts^
		return true
	}

	TryCheckType := !(TreeNode^ nd) -> Type^
	{
		itT := GetModuleType(nd.GetValueString("type"))
		if itT == null return null
		if nd.Childs.Size() == 2 {
			return itT
		}
		if nd.Childs.Size() == 3 {
			if IsReqList(nd,"110") //not nd.Childs[2].first
			{
				arrData := nd.Childs[2].second->{string}
				if arrData == "[4]" return itT.GetArray(4)
				if arrData == "[3]" return itT.GetArray(3)
			}
			if IsReqList(nd,"111") //nd.Childs[0].first and nd.Childs[1].first and nd.Childs[2].first
			{
				asTest := nd.Childs[2].second->{TreeNode^}
				if asTest.NodeName != "comment" return null
				return itT
			}
			if IsReqList(nd,"101")
			{
				postType := nd.Childs[1].second->{string}
				if postType[0] == '*' and postType[1] == ' '{
					return itT.GetPoint()
				}
				if postType == "** " return itT.GetPoint().GetPoint()
			}
		}
		if nd.Childs.Size() == 4{
			if (not nd.Childs[0].first) and not nd.Childs[2].first{
				preType := nd.Childs[0].second->{string}
				postType := nd.Childs[2].second->{string}
				
				if preType == "const " and postType[0] == '*' and postType[1] == ' '{
					return itT.GetPoint()
				}
			}
			if IsReqList(nd,"0101")
			{
				preType := nd.Childs[0].second->{string}
				postType := nd.Childs[2].second->{string}
				if preType == "const " and IsStrPointer(postType){
					return itT.GetPoint()
				}
			}
			if nd.Childs[0].first and (not nd.Childs[1].first) and nd.Childs[2].first and nd.Childs[3].first
			{
				postType := nd.Childs[1].second->{string}
				if IsStrPointer(postType)
				{
					return itT.GetPoint()
				}
			}
			if IsReqList(nd,"1101")
			{
				arrData := nd.Childs[2].second->{string}
				if arrData == "[4]" return itT.GetArray(4)
				if arrData == "[3]" return itT.GetArray(3)
				if arrData == "[2]" return itT.GetArray(2)
			}
		}
		if nd.Childs.Size() == 5{
			if (not nd.Childs[0].first) and nd.Childs[1].first and (not nd.Childs[2].first)
			and nd.Childs[3].first and nd.Childs[4].first
			{
				mConst := nd.Childs[0].second->{string}
				mPP := nd.Childs[2].second->{string}

				if mConst == "const " and mPP == "* const*      "{
					itTp := GetModuleType(nd.GetValueString("type"))
					if itTp == null return null
					return itTp.GetPoint().GetPoint()
				}
				if mConst == "const " and IsStrPointer(mPP){
					itTp := GetModuleType(nd.GetValueString("type"))
					if itTp == null return null
					return itTp.GetPoint().GetPoint()
				}
			}
			if IsReqList(nd,"11010")
			{
				fConst := nd.Childs[2].second->{string}
				sConst := nd.Childs[4].second->{string}

				if fConst == "[" and sConst == "]"
				{
					asVNod := nd.Childs[3].second->{TreeNode^}
					if asVNod.NodeName == "enum"
					{
						itTp := GetModuleType(nd.GetValueString("type"))
						itConst := nd.GetValueString("enum")
						intRes := 0
						if not GetFromInts(itConst,intRes&)
							return null
						return itTp.GetArray(intRes)
					}
				}
			}
		}
		if nd.Childs.Size() == 6{
			if (not nd.Childs[2].first) and (not nd.Childs[4].first) and nd.Childs[3].first
			{
				mbArrayPre := nd.Childs[2].second->{string}
				mbItem := nd.Childs[3].second->{TreeNode^}
				mbArrayPost := nd.Childs[4].second->{string}

				if mbItem.NodeName == "enum" and mbArrayPre == "[" and mbArrayPost == "]"
				{
					itVal := mbItem.Childs[0].second->{string}
					asInt := int
					if not GetFromInts(itVal,asInt&)
						return null
					if asInt <= 0 return null
					return itT.GetArray(asInt)
				}
			}
		}

		return null
	}

	TryGetStruct := !(string name) -> Type^
	{
		inMap := Structs.TryFind(name)
		if inMap != null
		{
			newStruct := new BoxClass(null,null,null)
			Typedefs[name] = newStruct.ClassType

			sTypVal := char^()

			for mems : inMap^.Childs
			{
				if mems.first{
					asN := mems.second->{TreeNode^}
					if asN.NodeName == "member"
					{
						memType := TryCheckType(asN)
						if memType == null {
							printf("VKMODULE: in struct %s need check %s\n",name,asN.GetValueString("name"))
							return null 
						}
						fieldName := asN.GetValueString("name")
						new FieldParam(fieldName,memType,newStruct)

						if fieldName == "sType"
						{
							inMapValues := asN.Attrs.TryFind("values")
							if inMapValues != null
							{
								sTypVal = inMapValues^
							}
						}
					}else{
						if asN.NodeName != "comment"
						{
							printf("VKMODULE: need check structure %s , node %s\n",name,asN.NodeName)
						}
					}
				}
			}



			if sTypVal != null
			{
				sTypeValue := GetItem(sTypVal)
				if sTypeValue == null or sTypeValue.GetValue() != "~int"
				{
					printf("WARN: structure %s does not have create value\n",name)
				}else{
					resPre := sTypeValue->{ObjInt^}
					resVal := resPre.MyInt
					itTyp := newStruct.ClassType
					newId := GetNewId()
					newName := "Func" + newId
					newFunc := new BuiltInFuncUno(newName,itTyp,true,GTypeVoid,false,
						"%Ptr## = getelementptr "sbt + itTyp.GetName() + " , " + itTyp.GetName() + "* #1 , i32 0, i32 0\n"
						+ "store i32 " + resVal + ", i32* %Ptr##\n" )

					GlobalStrs.Push("define void @"sbt + newName + "(" + itTyp.GetName() + "* %this"+newId+") \n"+
						"{\n" +
						"%Ptr"+newId+" = getelementptr "sbt + itTyp.GetName() + " , " + itTyp.GetName() + "* %this"+newId+" , i32 0, i32 0\n"
						+ "store i32 " + resVal + ", i32* %Ptr"+newId+"\n" 
						+ "ret void\n"
						+ "}\n")

					kindaBlock := new Object
					kindaBlock.Down = new ObjParam("this",false)
					kindaBlock.Down.Up = kindaBlock
					kindaBlock.Down.Down = newFunc
					newFunc.Up = newStruct.Down
					newStruct.Down = kindaBlock
					kindaBlock.Up = newStruct
				}
			}




			return newStruct.ClassType
		}
		
		return null
	}
	GetTypes := !(string name) -> Object^
	{
		inDefs := Typedefs.TryFind(name)
		if inDefs != null return new ObjType(inDefs^)

		return null
	}
	CheckProto := !(TreeNode^ nd) -> Type^
	{
		if nd.Childs.Size() == 2{
			itTypeName := nd.GetValueString("type")
			return GetModuleType(itTypeName)
		}
		return null
	}
	CheckCommandParam := !(TreeNode^ nd) -> Type^
	{
		if nd.Childs.Size() == 2{
			itTypeName := nd.GetValueString("type")
			return GetModuleType(itTypeName)
		}
		if nd.Childs.Size() == 3{
			if not nd.Childs[1].first
			{
				asSt := nd.Childs[1].second->{string}
				if asSt == "* "{
					asTp := GetModuleType(nd.GetValueString("type"))
					if asTp == null return null
					return asTp.GetPoint()
				}
				if asSt == "** "{
					asTp := GetModuleType(nd.GetValueString("type"))
					if asTp == null return null
					return asTp.GetPoint().GetPoint()
				}
				
			}
		}
		if nd.Childs.Size() == 4{
			if nd.Childs[0].first
			{
			}else{
				fSt := nd.Childs[0].second->{string}
				if nd.Childs[1].first
				{
					sNd := nd.Childs[1].second->{TreeNode^}
					if nd.Childs[2].first
					{
					}else{
						tSt := nd.Childs[2].second->{string}
						if nd.Childs[3].first
						{
							fNd := nd.Childs[3].second->{TreeNode^}

							if fSt == "const " and tSt == "* "{
								itTypeName := GetModuleType(nd.GetValueString("type"))
								if itTypeName == null return null
								return itTypeName.GetPoint()
							}
						}else{
						}
					}
				}else{
				}
			}
		}
		return null
	}
	GetTryFuncPointer := !(string name) -> Type^
	{	
		if name[0..4] != "PFN_" return null
		smplName := name[4..0].Str()
		inCmds := Commands.TryFind(smplName)

		if inCmds == null return null

		retType := null->{Type^}
		itms := Queue.{Type^}()
		for inCmds^.Childs
		{
			if it.first
			{
				asN := it.second->{TreeNode^}
				switch asN.NodeName
				{
					case "proto"
						retType = CheckProto(it.second->{TreeNode^})
						if retType == null {
							printf("VKMODULE: can not create func pointer %s ... proto\n",name)
							return null
						}
					case "param"
						resPar := CheckCommandParam(it.second->{TreeNode^})
						if resPar == null {
							printf("VKMODULE: can not create func pointer %s ... param %i\n",name,itms.Size() + 1)
							return null
						}
						itms.Push(resPar)
					case "implicitexternsyncparams"
						// nothing
					case void
						printf("VKMODULE: can not create func pointer %s\n",name)
						return null
				}
			}else{
				printf("VKMODULE: can not create func pointer %s\n",name)
				return null
			}
		}
		itPreType := GetFuncType(itms,null->{bool^},retType,false,false)
		return itPreType.GetPoint()
		
		if inCmds != null
			printf("VKMODULE: can not create func pointer %s\n",name)
		return null
	}
	GetModuleType := virtual !(string name) -> Type^
	{
		inDefs := Typedefs.TryFind(name)
		if inDefs != null return inDefs^

		maybeStruct := TryGetStruct(name)
		if maybeStruct != null return maybeStruct

		maybeCommand := GetTryFuncPointer(name)
		if maybeCommand != null return maybeCommand

		return null
	}
	GetItem := virtual !(string name) -> Object^
	{
		if name == "VkFuncsHolderCount"
			return ParsCount
		if name == "VkFuncsHolderStrs"
			return ParsNames
		testTypes := GetTypes(name)
		if testTypes != null return testTypes

		intRes := int
		if not GetFromInts(name,intRes&){
			if Enums.TryFind(name) != null
				printf("VKMODULE: can not read item %s\n",name)
			return null
		}
		return new ObjInt(intRes)
	}
}

TryReadInt := !(string s, int^ res) -> bool {
	val := 0
	sign := s[0] == '-'
	k := 0
	if sign k += 1
	while s[k] != 0
	{
		if s[k] in '0'..'9'{
			val *= 10
			val += s[k] - '0'
		}else{
			return false
		}
		k += 1
	}
	res^ = val
	return true
}

addedFieldsOrder := Queue.{Pair.{int,string}}
InstanceArr := class extend BoxClass
{
	itmPtr := ModuleVulkan^
	addedFields := AVLSet.{string}
	this := !(ModuleVulkan^ p) -> void
	{	
		itmPtr = p
		this."BoxClass.this"(null,null,null)
	}
	PreAskField:= virtual !(string name) -> void 
	{
		if addedFields.Contain(name)
			return void
		
		fullStr := "PFN_" + name
		inWut := itmPtr.GetModuleType(fullStr)
		if inWut != null
		{
			new FieldParam(name,inWut,this&)

			newStrId := StrContainer.GetStringValue(name)
			addedFields.Insert(name)
			addedFieldsOrder.Emplace(newStrId,name)
		}else{
			EmitError("cant create class field " + name)
		}

	}
	PrintStruct := virtual !(sfile s) -> void
	{
		funcsCount := addedFieldsOrder.Size()
	
		s << "@T" << itmPtr.ParsCount.MainId << " = global i32 " << funcsCount << "\n"

		if addedFieldsOrder.Size() != 0
		{
			s << "@T" << itmPtr.ParsNames.MainId << "Pre = global [ " << funcsCount << " x i8*] [\n"
			for it,i : addedFieldsOrder
			{
				strLL := strlen(it.second) + 1
				if i != 0 s << ","
				s << "i8* getelementptr inbounds(["<< strLL <<" x i8],["<< strLL <<" x i8]* @Str" << it.first <<" ,i32 0, i32 0)"
			}
			s << "]\n"
			s << "@T" << itmPtr.ParsNames.MainId << "= global i8** getelementptr inbounds([ "<< funcsCount <<" x i8*] , ["<< funcsCount <<" x i8*]* @T"
				<< itmPtr.ParsNames.MainId <<"Pre , i32 0, i32 0)\n"
		}
		this."BoxClass.PrintStruct"(s)
	}
}
