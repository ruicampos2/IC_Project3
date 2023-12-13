all:
	@(if [ ! -d build ]; then mkdir build; fi; cd build;\
	  if [ ! -d programs ]; then mkdir programs; fi; cmake ..; make)

clean:
	rm -rf build ../programs