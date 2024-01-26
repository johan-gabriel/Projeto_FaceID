## Projeto_FaceID
Código fonte para controle da RP2040 em C

## Resumo
Firmware desenvolvido a partir da data: 05/08/2022 Sistema desenvolvido para controle da Raspberry Pi Pico, que possui o microcontrolador RP2040. O código fonte foi desenvolvido em linguagem C com auxílio da SDK da Raspberry Pi Pico, utilizando a ferramenta CMake.

O circuito é responsável por bloquear o giro da catraca ou torniquete, quando não há evento de acesso pelo FaceID. Se há um evento de acesso o FaceID, que é um sistema independente, acessa o banco de dados e verifica se este acesso é autorizado para passar. Havendo a confirmação e ela sendo satisfatória, o FaceID manda um pulso seco para o circuito de controle, liberando o giro para passagem. Se for feita a tentativa de girar após o acesso, sem liberar novamente, o sistema bloqueia o próximo giro.

## IMPORTANTE
O Firmware está sujeito a mudanças devido a testes em campo. Não é o código final. Firmwares que possuem E têm apenas conexão com rede, mas não estão completamente implementados. Firmwares que possuem QR têm implementação de QRcode, mas apenas um leitor. Aguardando segunda fase com 2 leitores.

## CONFIGURAÇÕES
Para configurar o tempo de liberação pelo secBox, entre nas configurações do FaceID e coloque o tempo que deseja que fique liberado. Após isso, acione o secBox e coloque o tempo de 1500 ms para que o acionamento não altere o tempo configurado.

Na configuração do FaceID no módulo FACE NV, enquanto o sistema está online é necessário configurar como C (catraca) para que não haja conflito no acionamento do secBox e o comando vindo do Catraca. Quando sem rede o sistema irá continuar funcionando, pois quando o FaceID perde comunicação com o módulo FACE NV, o mesmo aciona o secBox.
