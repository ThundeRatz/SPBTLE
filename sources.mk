THIS_PATH := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

C_INCLUDES +=                                                                      \
	-I$(CUBE_DIR)/Middlewares/ST/X-CUBE-BLE1_BlueNRG-MS/includes                    \
	-I$(CUBE_DIR)/Middlewares/ST/X-CUBE-BLE1_BlueNRG-MS/hci/hci_tl_patterns/Basic   \
	-I$(CUBE_DIR)/Middlewares/ST/X-CUBE-BLE1_BlueNRG-MS/hci/                        \
	-I$(CUBE_DIR)/Middlewares/ST/X-CUBE-BLE1_BlueNRG-MS/utils                       \
	-I$(THIS_PATH)/inc                                                              \

LIB_SOURCES += $(shell find $(THIS_PATH) -name "*.c")

undefine THIS_PATH
