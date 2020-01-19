ARM = arm-unknown-linux-gnu-g++
OBJS = sockop
TARGETS = test

all: $(TARGETS)

$(OBJS).o: $(OBJS).c
	$(ARM) -c -o $@ $< -L /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/ -I /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/ -I /home/kai/microtime/linux/include/ -lpthread

test: test.c $(OBJS).o
	$(ARM) -o test $^ -L /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/ -I /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/ -I /home/kai/microtime/linux/include/ -lpthread

clean:
	rm -f *.o $(TARGETS)
