#import "Module.cp"
#mport "Type.cp"

EnumsValue := 1
EnumsBitPos := 2

ModuleVulkan := class extend CompilerModule
{
	xmlTree := TreeNode^
	Enums := AVLMap.{string,Pair.{int,string}}
	Typedefs := AVLMap.{string,Type^}
	ItInts := AVLMap.{string,int}

	Structs := AVLMap.{string,TreeNode^}

	this := !(string p) -> void
	{
		xmlFile := MappedFile(p)
		xmlTree = ParseXML(xmlFile.point)

	}
	InitModule := virtual !() -> bool
	{
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
				}
			}else{
			}
		}
		return true
	}
	CheckTypes := !(TreeNode^ nd) -> void
	{

		for typ : nd.Childs //structs only
		if typ.first
		{
			asNeed := typ.second->{TreeNode^}
			category := asNeed.Attrs.TryFind("category")
			if category != null and category^ == "struct"
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
						f  asNeed.Childs.Size() != 0 not asNeed.Childs[0].first and asNeed.Childs[0].second->{string} == "typedef "
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
						funcTypes := Queue.{Type^}()

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
		if nd.Childs.Size() == 4{
			if (not nd.Childs[0].first) and not nd.Childs[2].first{
				preType := nd.Childs[0].second->{string}
				postType := nd.Childs[2].second->{string}
				
				if preType == "const " and postType[0] == '*' and postType[1] == ' '{
					return itT.GetPoint()
				}
			}
		}
		if nd.Childs.Size() == 3 {
			if not nd.Childs[1].first
			{
				postType := nd.Childs[1].second->{string}
				if postType[0] == '*' and postType[1] == ' '{
					return itT.GetPoint()
				}
			}
			if not nd.Childs[2].first
			{
				arrData := nd.Childs[2].second->{string}
				if arrData == "[4]" return itT.GetArray(4)
				if arrData == "[3]" return itT.GetArray(3)
			}
			if nd.Childs[0].first and nd.Childs[1].first and nd.Childs[2].first
			{
				asTest := nd.Childs[2].second->{TreeNode^}
				if asTest.NodeName != "comment" return null
				return itT
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
						new FieldParam(asN.GetValueString("name"),memType,newStruct)
					}else{
						printf("VKMODULE: need check structure %s\n",name)
					}
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
	GetModuleType := virtual !(string name) -> Type^
	{
		inDefs := Typedefs.TryFind(name)
		if inDefs != null return inDefs^

		maybeStruct := TryGetStruct(name)
		if maybeStruct != null return maybeStruct

		return null
	}
	GetItem := virtual !(string name) -> Object^
	{
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
