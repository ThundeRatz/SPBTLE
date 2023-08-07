set(BLE_PATH
    ./lib/SPBTLE
)

list(APPEND LIB_SOURCES
    ${BLE_PATH}/src/spbtle.c
)

list(APPEND LIB_INCLUDE_DIRECTORIES
    ${BLE_PATH}/inc
    ./cube/Middlewares/ST/X-CUBE-BLE1_BlueNRG-MS/includes                  
	./cube/Middlewares/ST/X-CUBE-BLE1_BlueNRG-MS/hci/hci_tl_patterns/Basic 
	./cube/Middlewares/ST/X-CUBE-BLE1_BlueNRG-MS/hci/                      
	./cube/Middlewares/ST/X-CUBE-BLE1_BlueNRG-MS/utils                     
)