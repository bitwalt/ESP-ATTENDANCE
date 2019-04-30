deps_config := \
	/home/Stefano/esp/esp-idf/components/app_trace/Kconfig \
	/home/Stefano/esp/esp-idf/components/aws_iot/Kconfig \
	/home/Stefano/esp/esp-idf/components/bt/Kconfig \
	/home/Stefano/esp/esp-idf/components/driver/Kconfig \
	/home/Stefano/esp/esp-idf/components/esp32/Kconfig \
	/home/Stefano/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/home/Stefano/esp/esp-idf/components/esp_http_client/Kconfig \
	/home/Stefano/esp/esp-idf/components/ethernet/Kconfig \
	/home/Stefano/esp/esp-idf/components/fatfs/Kconfig \
	/home/Stefano/esp/esp-idf/components/freertos/Kconfig \
	/home/Stefano/esp/esp-idf/components/heap/Kconfig \
	/home/Stefano/esp/esp-idf/components/http_server/Kconfig \
	/home/Stefano/esp/esp-idf/components/libsodium/Kconfig \
	/home/Stefano/esp/esp-idf/components/log/Kconfig \
	/home/Stefano/esp/esp-idf/components/lwip/Kconfig \
	/home/Stefano/esp/esp-idf/components/mbedtls/Kconfig \
	/home/Stefano/esp/esp-idf/components/mdns/Kconfig \
	/home/Stefano/esp/esp-idf/components/mqtt/Kconfig \
	/home/Stefano/esp/esp-idf/components/nvs_flash/Kconfig \
	/home/Stefano/esp/esp-idf/components/openssl/Kconfig \
	/home/Stefano/esp/esp-idf/components/pthread/Kconfig \
	/home/Stefano/esp/esp-idf/components/spi_flash/Kconfig \
	/home/Stefano/esp/esp-idf/components/spiffs/Kconfig \
	/home/Stefano/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/Stefano/esp/esp-idf/components/vfs/Kconfig \
	/home/Stefano/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/Stefano/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/Stefano/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/Stefano/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/Stefano/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
