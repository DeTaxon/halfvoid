#import "MappedFile.cp"

XMLNode := class
{
	itValue := string
	attrs := AVLMap.{string,string}
	childs := Queue.{XMLNode^}

	this := !() -> void
	{
		itValue = ""
		attrs."this"()
		childs."this"()
	}
}

hiddenLoadXMLFile := !(Queue.{XMLNode^} nodes,u8^ ptr, int itSize) -> void
{
	
}


LoadXMLFile := !(string fileName) -> XMLNode^
{
	itFile := MappedFile(fileName)

	if itFile.IsInvalid() return null

	itRes := new XMLNode()
	hiddenLoadXMLFile(itRes.childs,itFile.point,itFile.Size())
	return itRes
}


