
CmpNameAndWildcard := !(char^ fileName,char^ inWild) -> bool
{
	j := 0
	k := 0

	while inWild[k] != 0
	{
		if inWild[k] == '*'
		{
			numAfter := inWild[k+1]
			if numAfter == 0 return true

			while fileName[j] != 0
			{
				if fileName[j] == numAfter and CmpNameAndWildcard(fileName[j]&,inWild[k+1]&)
					return true
				j += 1
			}
			return false
			//k += 1
		}else
		{
			if fileName[j] != inWild[k]
				return false
			j += 1
			k += 1
		}
	}
	return fileName[j] == 0
}
Wildcard := class
{
	toParse := char^
	this := !(char^ toIt) -> void
	{
		toParse = toIt
	}
	"~For" := !() -> DirectoryIterator2
	{
		return DirectoryIterator2(toParse,true)
	}

}

