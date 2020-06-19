# udp-safe-file-share

## Descrição do projeto

Foi desenvolvida uma aplicação em linguagem C para realizar o compartilhamento de arquivos entre hospedeiros dentro de uma mesma rede, inspirado no bit torrent. Ela funciona estabelecendo uma conexão UDP entre os hospedeiros, ficando sob responsabilidade da aplicação realizar o transporte confiável ( através do número de sequência, checksum e temporizador) de dados. Um hospedeiro se conecta ao rastreador e solicita um arquivo e o rastreador devolve o endereço de IP e porta de quem possui aquele arquivo. Como os hospedeiros são executados na mesma máquina, foi utilizado um sistema de diretórios para representá-los. Ao receber o endereço e a porta o hospedeiro solicitante estabelece a conexão e ocorre a transferência de arquivos.

## Guia de Execução

*Abrir o prompt de comando na pasta do rastreador;

*Executar o seguinte comando:
``
$ gcc Rastreador.c -o rastreadorudp
``
*Em seguida, executar o programa através do comando:
$ ./rastreadorudp
Esses comandos farão com que o rastreador espere por uma colicitação



gcc PackageSend.c -o send -l ws2_32 utils/binaryFunctions.c
