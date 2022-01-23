KERNEL_SRC += $(shell find kernel/core/src -iname *.cpp)
KERNEL_INC += kernel/core/inc

MOD_JAVA_BASE_SRC = $(shell find kernel/core/java/java.base -iname *.java)
MOD_JAVA_BASE_OBJ = $(JAVA_SRC:kernel/core/java/java.base/%.java=$(BASE_BIN_DIR)/java/core/%.class)
JAVA_SRC += $(MOD_JAVA_BASE_SRC)
JAVA_OBJ = $(MOD_JAVA_BASE_OBJ)

ICB_CLASSES += $(JAVA_OBJ)
ICB_FILE = $(BIN_DIR)/core.icb

.PHONY: build_icb
build_icb: build_java
	./gradlew :tools:icb-tool:run --args="$(ICB_FILE) $(JAVA_OBJ)"

$(ICB_FILE): build_icb

.PHONY: build_java
build_java:
	javac $(JAVA_SRC)
#./gradlew classes
