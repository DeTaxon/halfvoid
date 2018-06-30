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
		poses[0] = 0
		poses[1] = 0

		pos := 0
		working := true

		while  working and pos < daFile.Size()
		{
			nowVal := daFile[pos]

			for i : 2
			{
				if keyWords[i][poses[i]] == nowVal
				{
					poses[i] += 1
					if keyWords[i][poses[i] ] == '!'
					{
						if i == 0
						{
							newVal := 0
							pos += 1

							while daFile[pos] in '0'..'9'
							{
								newVal = newVal*10 + daFile[pos] - '0'
								pos += 1
							}
							pos -= 1
							printf("got %i\n",newVal)
							

						}else{
							working = false
						}
					}
				}
			}

			
			pos += 1
		}


		return true
	}
}




