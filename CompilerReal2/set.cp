
Set := class .{@T} extend Stack.{T}
{
	Insert := !(T a) -> void
	{
		iter := Start
		found := false

		while iter != null
		{
			if iter.Data == a
			{
				found = true
				iter = null
			}else{
				iter = iter.Next
			}
		}
		if not found Push(a)
	}
}

