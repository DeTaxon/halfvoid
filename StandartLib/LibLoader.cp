
intrRepoBrowser := vRepo^

LibDBObject := class
{
	hndl := void^
	itFileName := char^
	loadedFuncs := AVLMap.{char^,void^}
	if $win32
		win32DllData := win32DllLoadData^

	Get := !(char^ name) -> void^
	{
		inMap := loadedFuncs.TryFind(name)
		if inMap != null
			return inMap^
		

		LibDB.itMemPool.Push()
		defer LibDB.itMemPool.Pop()

		if $win32 if win32DllData != null
		{
			inExport := win32DllData.exportFunctions.TryFind(name)
			if inExport != null
			{
				loadedFuncs[StrCopy(name)] = inExport^
				return inExport^
			}
		}
	
		newFunc := LoadFuncLib(hndl,name)
		if newFunc  == null
			throw new Exception(0,"Can not load function <"sbt + name + ">")
		loadedFuncs[StrCopy(name)] = newFunc
		
		return newFunc
	}
}

Exdepth := 0
LibDatabaseType := class
{
	loadedLibs := AVLMap.{char^,LibDBObject}
	itMemPool := AllocOnlyMP.{4096,true}
	
	Clean := !() -> void
	{
		for loadedLibs
		{
			CloseLib(it.hndl)
			if it.itFileName != null
				TFSDelete(it.itFileName)
		}
	}
	SetRepo := !(vRepo^ toS) -> void
	{
		intrRepoBrowser = toS
	}
	GetLib := !(char^ name) -> LibDBObject^
	{
		inMap := loadedLibs.TryFind(name)
		if inMap != null
			return inMap

		itMemPool.Push()
		defer itMemPool.Pop()

		newObj := LibDBObject
	
		hndl := OpenLib(name)
		
		if hndl == null
		{
			exTest := Path("./"sbt + name)
			if exTest.IsExist()
			{
				hndl = OpenLib(exTest.FullPath().Get())
			}
		}

		if hndl != null
		{
			nameCpy := StrCopy(name)
			inMap2 := ref loadedLibs[nameCpy]
			inMap2.hndl = hndl
			return inMap2&
		}
		
		if intrRepoBrowser != null
		{
			inFS := intrRepoBrowser.GetFile(name)
			if inFS != null
			{
				if inFS.IsVirtual()
				{
					if $win32
					{

						pt := inFS.Map()
						defer inFS.Unmap()

						resLoad := win32LoadLibrary(pt,inFS.Size())

						nameCpy := StrCopy(name)
						inMap2 := ref loadedLibs[nameCpy]
						inMap2.hndl = hndl
						//inMap2.itFileName = StrCopy(tmpName)
						inMap2.win32DllData = resLoad
						return inMap2&
					}
					if $posix
					{
						pt := inFS.Map()
						defer inFS.Unmap()

						rf := RawFile()
						newName := rf.OpenTemp(name,true)
						rf.Write(pt,inFS.Size())
						rf.Close()

						hndl = OpenLib(newName)
						inMap4 := ref loadedLibs[StrCopy(name)]
						inMap4.hndl = hndl
						return inMap4&
					}
				}else{
					toLoad := inFS.GetPath()
					toLoad2 := Path(toLoad)
					toLoad = toLoad2.FullPath().Get()

					hndl = OpenLib(toLoad)


					if hndl == null
						return null
					inMap3 := ref loadedLibs[StrCopy(name)]
					inMap3.hndl = hndl
					return inMap3&
				}
			}
		}

		return null
	}
}
LibDB := LibDatabaseType

Library := class 
{
	hndl := LibDBObject^
	this := !(args...) -> void
	{
		Open(args...)
	}
	Open := !(char^ name,args...) -> void
	{
		hndl = LibDB.GetLib(name)
		if hndl == null
			Open(args...)
	}
	Open := !(char^ name) -> void
	{
		hndl = LibDB.GetLib(name)
		if hndl == null
			throw new Exception(0,"can not open library, like "sbt + name)
	}
	Open := !(char^^ names,int count) -> void
	{
		for i : count
		{
			hndl = LibDB.GetLib(names[i])
			if hndl != null
			{	
				return void
			}
		}
		throw new Exception(0,"can not open library, like "sbt + names[0])
	}
	TryOpen := !(char^ name) -> bool
	{
		hndl = LibDB.GetLib(name)
		return hndl != null
	}
	Get := !(char^ funcName) -> void^
	{
		return hndl.Get(funcName)
	}
}


