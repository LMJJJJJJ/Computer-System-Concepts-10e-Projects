obj-m += simple.o
obj-m += jiffiesproc.o
obj-m += secondsproc.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
