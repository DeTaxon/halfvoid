#import "lib.cp"
#import "MappedFile.cp"

MODEL_NONE := 0
MODEL_LOADED := 1
MODEL_INVRAM := 2

MODEL_VERTEX := 1
MODEL_NORMAL := 2
MODEL_UV := 4

Model := class
{
	id := int
	verts := float[]
	inds := int[]

	modelState := int

	this := !() -> void
	{
		verts = null
		inds = null
		id = 0

		modelState = MODEL_NONE
	}
	LoadFromPLY := !(char^ name) -> bool
	{	
		daFile := MappedFile(name)

		if daFile.IsInvalid() 
			return false
		
		keyWords := string[2]
		keyWords[0] = "element vertex !" 
		keyWords[1] = "end_header!"

		poses := int[2]
		for poses printf("it %i\n",it)

		pos := 0
		working := true

		while  working and pos < daFile.Size()
		{
			nowVal := daFile[pos]

			
			pos += 1
		}


		return true
	}
}




