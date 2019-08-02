# SPBTLE

Biblioteca para lidar com o módulo bluetooth [SPBTLE](https://www.st.com/en/wireless-transceivers-mcus-and-modules/spbtle-rf.html), com base no pacote X-CUBE-BLE1 disponível no STRM32CubeMX. 

## Adicionando o submódulo ao projeto

Crie um diretório chamado `lib`, caso não exista:

```bash
mkdir lib
```
E adicione o submódulo fazendo:

* Com HTTPS:
```bash
git submodule add --name SPBTLE https://github.com/ThundeRatz/SPBTLE.git lib/SPBTLE
```

* Com SSH:
```bash
git submodule add --name SPBTLE git@github.com:ThundeRatz/SPBTLE.git lib/SPBTLE
```

## Adicionando o pacote X-CUBE-BLE1 ao projeto

No seu projeto do STM32CubeMX, na aba `Additional Software`, abra `STMicroelectronics.X-CUBE-BLE1 > Wireless_BlueNRG-MS`, marque as caixas de `Controller` e `Utils`, e selecione `Basic` em `HCI_TL` e `UserBoard` em `HCI_TL_INTERFACE`. Depois, basta configurar o pacote no seu projeto, selecionando os pinos corretos que fazem a interface com o SPI do microcontrolador.

Um projeto exemplo pode ser encontrado em [freitas-renato/SPBTLE](https://github.com/freitas-renato/spbtle), feito com base no [STM32ProjectTemplate](https://github.com/ThundeRatz/STM32ProjectTemplate).


-------------

Equipe ThundeRatz de Robótica