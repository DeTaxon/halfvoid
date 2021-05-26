
taskTestValue := task_local int
gr := task_local double


TaskTest := !() -> void
{
	TExpectWorkers(1)
	//tb.Monitor(".",true,(x) ==>{
	//	printf("test %s\n",x)
	//})
	TSpawnTask(() ==> []{
		taskTestValue = 0
		for 4
		{
			printf("wow %i\n",taskTestValue)
			taskTestValue = taskTestValue + 1
			AwaitWork(() ==> {
				TSleep(1)
			})
		}
		assert(taskTestValue == 4)
		
	})
	TSpawnTask(() ==> {
		taskTestValue = 100
		for 2
		{
			printf("waw %i\n",taskTestValue++)
			TSleep(1)
		}
		assert(taskTestValue = 100)
		
	})
}
