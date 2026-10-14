# 0d1n-debian

Fork do **0d1n** preparado para **Debian/Parrot OS**, com empacotamento `.deb` nativo.

## Creditos

| Papel | Nome |
|-------|------|
| Autor original | **Antonio Carlos Costa da Silva** (CoolerVoid / Cooler_) |
| Projeto upstream | https://github.com/CoolerVoid/0d1n |
| Mantenedor deste fork | **Rhaniere Vieira** |

Veja tambem o arquivo `NOTICE` e a licenca em `LICENSE`.

## O que este fork adiciona

* Empacotamento Debian nativo (`debian/`, `dpkg-buildpackage`)
* Flag `--json <arquivo>` para exportar resultados em JSON
* Flag `--delay <ms>` para rate-limiting entre requisicoes
* Nome do binario/pacote: `0d1n-debian`
* Dados instalados em `/opt/0d1n-debian/`

## Dependencias

```bash
sudo apt-get install libcurl4-openssl-dev debhelper
```

## Compilar

```bash
cd 0d1n-OdinV38
make
./bin/0d1n-debian --help
```

## Gerar e instalar o .deb (Parrot/Debian)

```bash
cd 0d1n-OdinV38
dpkg-buildpackage -us -uc -b
sudo dpkg -i ../0d1n-debian_3.8-1_amd64.deb
sudo apt install -f   # se faltar dependencia
which 0d1n-debian     # deve apontar para /usr/bin/0d1n-debian
```

## Instalacao manual (sem .deb)

```bash
make
sudo make install
# binario: /usr/local/bin/0d1n-debian
# dados:   /opt/0d1n-debian/
```

## Viewer (opcional)

O viewer continua derivado do upstream. Paths de dados apontam para `/opt/0d1n-debian/`.

```bash
cd 0d1n_viewer
make
# ajuste a instalacao conforme o Makefile do viewer
```

## Exemplos basicos

```bash
0d1n-debian --host 'http://127.0.0.1/^' \
  --payloads /opt/0d1n-debian/payloads/dir_brute.txt \
  --threads 10 --timeout 3 --log teste1 \
  --json /tmp/resultado.json --delay 200
```

Proxy (Burp/ZAP) ja era suportado no original:

```bash
0d1n-debian --host 'http://127.0.0.1/^' \
  --payloads /opt/0d1n-debian/payloads/dir_brute.txt \
  --proxy http://127.0.0.1:8080 --threads 1
```

## Funcionalidades herdadas do 0d1n original

* Forca bruta de formularios e diretorios
* Testes de injecao com wordlists
* Anti-CSRF token, proxy e user-agent aleatorios
* Thread pool em C + libcurl

Video demo do projeto original: https://www.youtube.com/watch?v=1L22mbbVge0

## Desinstalacao

```bash
sudo make uninstall
# ou, se instalado via dpkg:
sudo dpkg -r 0d1n-debian
```

## Licenca

Redistribuicao sujeita a licenca do projeto original (ver `LICENSE`). Este fork mantem os avisos de copyright do autor original.
