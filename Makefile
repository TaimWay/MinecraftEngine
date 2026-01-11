# Test
COMPILER = g++
STANDAND = c++17
PARAMETER = -O2 -DMINECRAFT_ENGINE_IMPLEMENTATION 
OUTPUT = output/
INCLUDE = src

default:
	echo Emm...

test.java:
	$(COMPILER) src/test/java.cpp -std=$(STANDAND) -o $(OUTPUT)test.exe -I $(INCLUDE) $(PARAMETER)

test.version:
	$(COMPILER) src/test/java.cpp -std=$(STANDAND) -o $(OUTPUT)test.exe -I $(INCLUDE) $(PARAMETER)

test.config:
	$(COMPILER) src/test/config.cpp -std=$(STANDAND) -o $(OUTPUT)test.exe -I $(INCLUDE) $(PARAMETER)
	
run.test:
	$(OUTPUT)test.exe

run.clean:
	rm $(OUTPUT)test.exe
