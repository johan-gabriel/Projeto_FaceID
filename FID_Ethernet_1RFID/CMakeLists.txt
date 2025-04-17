cmake_minimum_required(VERSION 3.13)

set(CURRENT_PROJECT_VERSION "0.1")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# O diretório onde está o SDK, neste caso apontando para duas pastas abaixo.
# Você pode também utilizar o caminho absoluto, mas isso não é recomendado.
# O caminho aqui é relativo à pasta ./build do projeto.
set(PICO_SDK_PATH "../../../../pico-sdk")

# Inclui o SDK do Raspberry Pi Pico no projeto.
include(pico_sdk_import.cmake)

# Define o nome do projeto.
# Lembre-se de trocar para o nome do seu projeto,
# após a troca, se houver uma pasta build, remova e rode o comando Configure novamente.
project(2RFID C CXX ASM)

# Inicializa o SDK do Raspberry Pi Pico.
pico_sdk_init()

# Aqui devemos incluir uma lista dos arquivos que serão compilados no nosso executável,
# geralmente contendo pelo menos main.c

set(PROJECT_SOURCE_DIR src)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME}

    ${PROJECT_SOURCE_DIR}/hw_config.c
    ${PROJECT_SOURCE_DIR}/CH9121.c
    ${PROJECT_SOURCE_DIR}/SP.c
    ${PROJECT_SOURCE_DIR}/storage.c
    ${PROJECT_SOURCE_DIR}/main.c
)

# Alguns metadados do projeto
pico_set_program_name(${PROJECT_NAME} ${PROJECT_NAME})
pico_set_program_version(${PROJECT_NAME} ${CURRENT_PROJECT_VERSION})

# Habilita o uso da porta serial como saída para printf/puts.
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# Desabilita o uso da porta USB como serial (como vamos usar o PicoProbe, podemos deixar desligado).
# Caso queira ver a saída serial de puts e printf via USB, troque de 0 para 1.
pico_enable_stdio_usb(${PROJECT_NAME} 1)

set(N_SD_CARDS 1)
add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI)

# Aqui devemos incluir todas as bibliotecas que estamos utilizando no firmware,
# terá sempre no mínimo a biblioteca padrão do Pico (pico_stdlib).
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    FatFs_SPI
)

# Faz com que o build gere arquivos extras para o Pico (.uf2, por exemplo).
pico_add_extra_outputs(${PROJECT_NAME})