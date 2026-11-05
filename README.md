# Implementação 5 · Sincronização do Metrô

Infraestrutura de Software · CESAR School · `bgs@cesar.school`

[Relatório em PDF](bgs.pdf) · [Pacote para entrega](bgs.tar) · [Registro de testes](evidencias.log)

Cada estação coordena as threads de um vagão e de seus passageiros. A reserva de um assento e a confirmação do embarque são operações separadas. Assim, vários passageiros podem embarcar ao mesmo tempo, e o vagão só parte quando todas as reservas foram confirmadas e a fila acabou ou as vagas se esgotaram.

## Compilar e executar

O código foi preparado em Windows com WSL 2 e compilado e testado em **Ubuntu 26.04 LTS**, com GCC 15.2.0 e GNU Make 4.4.1. Requer um compilador C e a biblioteca POSIX Threads.

Para trabalhar a partir da entrega, execute:

```sh
tar -xf bgs.tar
cd bgs
make
./sync_car
```

Se você clonou o repositório, execute `make` e `./sync_car` no diretório que contém o Makefile. O executável usa o runner fornecido e termina com `Looks good!` quando todos os 100 passageiros são contabilizados. Os números por vagão variam a cada execução.

## Testar e limpar

```sh
make test     # executa o runner e o cenário com duas estações
make clean    # remove os executáveis e os objetos gerados pelo make
```

O segundo teste verifica que uma capacidade inválida (`-1`) retorna sem conceder uma vaga. Em seguida, um passageiro reserva em A enquanto o primeiro ainda aguarda confirmação. A estação B conclui uma viagem enquanto A ainda aguarda o segundo embarque. O teste encerra com `OK: duas reservas simultâneas e duas estações independentes.` em caso de sucesso.

## Arquivos

| Arquivo | Função |
| --- | --- |
| `sync_car.c` | Inicialização da estação, chegada do vagão, reserva de vaga e confirmação de embarque. |
| `sync_car.h` | Definição de `struct station` e protótipos compartilhados com os testes. |
| `sync_car-runner.c` | Runner fornecido com a atividade. Testa vagões vazios, vagões sem vagas e viagens com capacidades aleatórias. |
| `test_stations.c` | Teste complementar de embarque simultâneo, chegada de um passageiro durante o embarque e independência entre duas estações. |
| `Makefile` | Gera `sync_car`, executa os testes e limpa os arquivos compilados. |
| `bgs.pdf` | Relatório da implementação e dos testes. |
| `evidencias.log` | Sessão de validação gravada com `script` no Ubuntu. Contém os comandos, suas saídas e as etapas datadas E1 a E7. |
| `evidencias/erro.png` e `evidencias/validacao.png` | Capturas do painel de terminal durante a compilação estrita e a execução final dos testes. |
