all: threadPoolTest channelTest futureTest

threadPoolTest: threadPool/main.c threadPool/threadPool.c myQueue/myQueue.c 
	gcc -o threadPool/threadPoolTest -g threadPool/main.c myQueue/myQueue.c threadPool/threadPool.c -lpthread

channelTest: channel/main.c channel/channel.c myQueue/myQueue.c
	gcc -o channel/channelTest -g channel/main.c channel/channel.c myQueue/myQueue.c -lpthread 

futureTest: future/main.c future/future.c channel/channel.c myQueue/myQueue.c
	gcc -o future/futureTest -g future/main.c  future/future.c channel/channel.c myQueue/myQueue.c -lpthread