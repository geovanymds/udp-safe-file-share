# udp-safe-file-share

## 1 Descrição do projeto

Foi desenvolvida uma aplicação em linguagem C para realizar o compartilhamento de arquivos entre hospedeiros dentro de uma mesma rede, inspirado no bit torrent. Ela funciona estabelecendo uma conexão UDP entre os hospedeiros, ficando sob responsabilidade da aplicação realizar o transporte confiável ( através do número de sequência, checksum e temporizador) de dados. Um hospedeiro se conecta ao rastreador e solicita um arquivo e o rastreador devolve o endereço de IP e porta de quem possui aquele arquivo. Como os hospedeiros são executados na mesma máquina, foi utilizado um sistema de diretórios para representá-los. Ao receber o endereço e a porta o hospedeiro solicitante estabelece a conexão e ocorre a transferência de arquivos.

## 2 Guia de Execução

### 2.1 Executando o rastreador

* Abrir o prompt de comando na pasta do rastreador;
* Executar o seguinte comando: </br>
`
$ gcc Rastreador.c -o rastreadorudp
`
* Em seguida, executar o programa através do comando: </br>
`
$ ./rastreadorudp
`
Esses comandos farão com que o rastreador espere por uma solicitação. </br>


### 2.2 Executando um servidor

* Abrir um segundo prompt na pasta de um dos servidores; </br>
* Executar o comando: </br>
`
$ gcc Servidor.c -o servidorudp
`
* Em seguida executar o programa gerado: </br>
`
$ ./servidorudp
`
Esses comandos farão com que o servidor aguarde a colicitação por um de seus arquivos.

### 2.3 Executando o cliente

* Abrir um terceiro terminal na pasta raíz e executar o seguinte comando: </br>
`
$ - gcc Cliente.c -o clienteudp 
`
* Em seguida executar o programa gerado: </br>
`
$ ./clienteudp 127.0.0.1 "NomeDoArquivo"
`
</br>
OBS: Pode ser executado mais de um servidor ao mesmo tempo, bastando repetir o passo 1.2 para os demais servidores. Isto pode ser útil para observar o comportamento do programa com a presença de mais hospedeiros. </br>
O arquivo que deseja receber deve estar presente na pasta do servidor(es) em execução.

