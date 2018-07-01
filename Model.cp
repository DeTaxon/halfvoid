#import "lib.cp"
#import "MappedFile.cp"

MODEL_NONE := 0
MODEL_LOADED := 1
MODEL_INVRAM := 2

MODEL_POSITION := 1
MODEL_NORMAL := 2
MODEL_UV := 4

Model := class
{
	id := int
	verts := float[]
	vertItems := int

	inds := int[]

	modelState := int

	this := !() -> void
	{
		verts = null
		inds = null
		id = 0
		vertItems = 0

		modelState = MODEL_NONE
	}
	LoadFromPLY := !(char^ name) -> bool
	{	
		daFile := MappedFile(name)

		if daFile.IsInvalid() 
			return false
		
		keyWords := string[3]
		keyWords[0] = "element vertex !" 
		keyWords[1] = "end_header!"
		keyWords[2] = "property float !"

		poses := int[3]
		poses[0] = 0
		poses[1] = 0
		poses[2] = 0

		pos := 0
		working := true

		vertsCount := 0

		while working and pos < daFile.Size()
		{
			nowVal := daFile[pos]

			for i : keyWords->len
			{
				if keyWords[i][poses[i]] == nowVal
				{
					poses[i] += 1
					if keyWords[i][poses[i] ] == '!'
					{
						if i == 0
						{
							pos += 1

							while daFile[pos] in '0'..'9'
							{
								vertsCount = vertsCount*10 + daFile[pos] - '0'
								pos += 1
							}
							pos -= 1
						}
						if i == 1 working = false
						if i == 2{	
							pos += 1
							if daFile[pos] == 'x' vertItems = vertItems or_b MODEL_POSITION
							if daFile[pos] == 'n' vertItems = vertItems or_b MODEL_NORMAL
							if daFile[pos] == 's' vertItems = vertItems or_b MODEL_UV
						}
						poses[i] = 0
					}
				}else{
					poses[i] = 0
				}
			}			
			pos += 1
		}

		vertSize := 0

		if vertItems and_b MODEL_POSITION vertSize += 3
		if vertItems and_b MODEL_NORMAL vertSize += 3
		if vertItems and_b MODEL_UV vertSize += 2

		if vertItems != 7 return false //TODO: normalless support

		verts = new float[vertsCount*vertSize]

		posInVerts := 0

		isNeg := false
		hiValue := 0
		lowValue := 1 // value = hi / lo

		vertFileState := 0

		while posInVerts < vertsCount and pos < daFile.Size()
		{
			if vertFileState == 0
			{
				switch daFile[pos]
				{
					case '-'
						isNeg = true
					case '0'..'9'
						hiValue = hiValue*10 + daFile[pos] - '0'
					case '.'
						vertFileState = 1
					case void
						isNeg = false
						lowValue = 1
						hiValue = 0
				}
			}
			if vertFileState == 1
			{
				if daFile[pos] in '0'..'9'
				{
					hiValue = hiValue*10 + daFile[pos] - '0'
					lowValue *= 10
				}else{
					verts[posInVerts] = hiValue / lowValue
					if isNeg  {
						verts[posInVerts] *= -1.0f
					}
					isNeg = false
					lowValue = 1
					hiValue = 0
					posInVerts += 1
					vertFileState = 0
				}
			}
			pos += 1
		}
		printf("wut %i %i %i\n",posInVerts,vertsCount)

		return true
	}
}




