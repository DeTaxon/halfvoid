
ObjFunc := class extend Object
{
	nameId := int
	ParNames := Queue.{string}
	this := !(Object^ root) -> void
	{
		Clean()
		ParNames.Start = null

		iter := root

		while iter.GetValue() != "()" iter = iter.Right
		
		ParseParams(iter.Down)	
	}
	ParseParams := !(Object^ pars) -> void
	{
		NowData := Queue.{Object^}()
		ParType := Queue.{Type^}()
		
		iter := pars

		while iter != null
		{
			if iter.GetValue() == ","
			{
				ShouldStr := NowData.Pop()

															
			}else
			{
				NowData.Push(iter)
			}
		}

	}
}

