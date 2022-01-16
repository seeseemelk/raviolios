TEST_ELF = $(KERNEL_BIN_DIR)/test.elf

KERNEL_SRC += $(shell find kernel/test/src -iname *.cpp)
KERNEL_INC += kernel/test/inc

KERNEL_OBJ = $(KERNEL_SRC:%=$(KERNEL_BIN_DIR)/%.o)

JAVA_SRC = $(shell find kernel/test/java -iname *.java)
JAVA_OBJ = $(JAVA_SRC:%.java=$(KERNEL_BIN_DIR)/%.class)

CFLAGS = -Wall -Wextra -Werror -pedantic $(KERNEL_INC:%=-I%) -ggdb \
	-fsanitize=address -O0
CPPFLAGS = $(CFLAGS) -std=c++20

.PHONY: kernel
kernel: $(TEST_ELF) $(JAVA_OBJ)
	$(TEST_ELF)
	
build_test: $(TEST_ELF)

$(TEST_ELF): $(KERNEL_OBJ)
	mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -o $@ $^

$(KERNEL_BIN_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -MP -MD -MF $@.d -c -o $@ $<

$(BIN_DIR)/java/%.class: kernel/test/java/%.java $(firstword $(JAVA_OBJ))

$(firstword $(JAVA_OBJ)): $(JAVA_SRC)
	mkdir -p $(dir $@)
	javac --release 11 -g:none -d $(dir $@) $(JAVA_SRC)

.PHONY: clean_test
clean: clean_test
clean_test:
	rm -f $(TEST_ELF) 

-include $(KERNEL_OBJ:%=%.d)