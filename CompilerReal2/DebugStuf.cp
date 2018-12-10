#import "Globals.cp"


CreateDebugCall := !(Object^ itm) -> int
{
	if itm.Line == null return -1
	if not DebugMode return -1

	itr := itm
	while itr != null and itr.GetValue() != "!()" itr = itr.Up

	if itr == null return -1

	asN := itr->{BoxFunc^}
	itId := GetNewId()
	DebugMetaData.Push("!" + itId + " = !DILocation(line:" + itm.Line.LinePos +", column:0 , scope:!" + asN.ABox.ItId + ")\n")
	return itId
}

