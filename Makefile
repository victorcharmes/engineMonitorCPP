# ---------------------------------------------------------------------------
#  Engine Monitor - exercice C++98
# ---------------------------------------------------------------------------
#  make               : compile le programme bin/monitor
#  make test-logger   : etape 1      make test-sensor   : etape 2
#  make test-registry : etape 3      make test-alarm    : etape 4
#  make test-parser   : etape 5
#  make scenarios     : etape 6 (tests de bout en bout)
#  make test          : tous les tests
#  make memcheck      : tous les tests sous valgrind (fuites memoire)
#  make clean
# ---------------------------------------------------------------------------

CXX      ?= g++
CXXFLAGS := -std=c++98 -Wall -Wextra -pedantic -Werror -g -Iinclude
BUILD    := build
BIN      := bin

# Objets necessaires a chaque etape (les tests ne compilent que ce dont ils ont besoin)
OBJ_LOGGER   := $(BUILD)/Logger.o
OBJ_SENSOR   := $(BUILD)/Sensor.o $(BUILD)/TemperatureSensor.o $(BUILD)/PressureSensor.o
OBJ_REGISTRY := $(OBJ_SENSOR) $(BUILD)/SensorRegistry.o
OBJ_ALARM    := $(BUILD)/Sensor.o $(BUILD)/AlarmManager.o
OBJ_PARSER   := $(BUILD)/CommandParser.o
OBJ_APP      := $(OBJ_LOGGER) $(OBJ_REGISTRY) $(BUILD)/AlarmManager.o $(OBJ_PARSER) \
                $(BUILD)/MonitoringSystem.o $(BUILD)/main.o

UNIT_TESTS := logger sensor registry alarm parser
VALGRIND   := valgrind --leak-check=full --error-exitcode=42 --quiet

.PHONY: all test unit scenarios memcheck clean $(addprefix test-,$(UNIT_TESTS))

all: $(BIN)/monitor

$(BIN)/monitor: $(OBJ_APP) | $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD)/%.o: src/%.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/test_%.o: tests/unit/test_%.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BIN)/test_logger:   $(BUILD)/test_logger.o   $(OBJ_LOGGER)   | $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(BIN)/test_sensor:   $(BUILD)/test_sensor.o   $(OBJ_SENSOR)   | $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(BIN)/test_registry: $(BUILD)/test_registry.o $(OBJ_REGISTRY) | $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(BIN)/test_alarm:    $(BUILD)/test_alarm.o    $(OBJ_ALARM)    | $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(BIN)/test_parser:   $(BUILD)/test_parser.o   $(OBJ_PARSER)   | $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(addprefix test-,$(UNIT_TESTS)): test-%: $(BIN)/test_%
	./$<

unit: $(addprefix test-,$(UNIT_TESTS))

scenarios: $(BIN)/monitor
	./tests/run_tests.sh $(BIN)/monitor

test: unit scenarios

memcheck: $(addprefix $(BIN)/test_,$(UNIT_TESTS)) $(BIN)/monitor
	@for t in $(UNIT_TESTS); do \
	    echo "== valgrind test_$$t"; \
	    $(VALGRIND) ./$(BIN)/test_$$t > /dev/null || exit 1; \
	done
	@for s in tests/scenarios/*.txt; do \
	    echo "== valgrind monitor $$s"; \
	    $(VALGRIND) ./$(BIN)/monitor $$s > /dev/null; \
	    [ $$? -ne 42 ] || exit 1; \
	done
	@echo "memcheck : aucune fuite ni erreur memoire"

$(BUILD) $(BIN):
	mkdir -p $@

clean:
	rm -rf $(BUILD) $(BIN)

-include $(wildcard $(BUILD)/*.d)
