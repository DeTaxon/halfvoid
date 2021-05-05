
WorkBag := PriorityStack.{Object^,20}

workIter := int
WorkWithBag := !(bool printW) -> void
{
	try
	{
		while (not WorkBag.IsEmpty()) and ErrorLog.Empty()
		{
			FlushTempMemory()
			prior := WorkBag.GetTopPriority()
			it := WorkBag.Pop()
			workingOnObject = it


			if printW
			{
				printf("itWork %i %s\n",workIter,it.GetValue())
				if it.Line != null {
					printf("at %s %i\n",it.Line.inFile.itStr,it.Line.LinePos)
					//it.Print(0)
					}
				workIter += 1
			}

			it.DoTheWork(prior)
		}
	}catch(IException^ e)
	{
		return void
	}
}