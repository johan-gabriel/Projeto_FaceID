# Projeto_FaceID
Código fonte para controle da RP2040 em c

## Resumo

Firmware desenvolvido apartir da data: 05/08/2022
Sistema desenvolvido para controle da Respberry Pi Pico, que possue o microcontrolador RP2040. O código fonte foi desenvolvido em linguagem C com auxilio da SDK da Raspberry Pi Pico, utilizando a ferramenta CMake.

O circuito é responsável por bloquear o giro da catraca ou torniquet, quando não há evento de acesso pelo FaceID. Se há um evento de acesso o FaceID que é um sistema independente acessa o banco de dados e verifica se este acesso é autorizado para passar, havendo a confirmação e ela sendo satisfatória o FaceID manda um pulso seco para o circuito de controle liberando o giro para passagem. Se for feito a tentativa de girar após o acesso, sem liberar novamente, o sistema bloqueia o proximo giro.

## IMPORTANTE

O Firmware está sugeito a midanças devido a teste em campo. Não é o código final.
