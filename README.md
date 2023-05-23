# Anemometr
Stacja mierząca prędkość wiatru oparta o płytkę ESP8266 i interfejs web wyświetlający na bieżąco wyniki pomiarów.
Projekt zespołowy został wykonany w ramach zajęć na kierunku Informatyka Przemysłowa na Filii JG Politechniki Wrocławskiej.

## Platforma
Moduł ESP8266 NodeMCU v2 został wybrany ze względu na:
- łączność z WiFi
- niską cenę układu ESP8266
- wbudowany konwerter UART - USB
- możliwość użycia bibliotek Arduino

## Środowisko
Płytkę ESP8266 można zaprogramować za pomocą Arduino IDE. Niestety nowsze wersje tego środowiska (v2+) są niekompatybilne z wieloma istniejącymi bibliotekami. W związku z tym projekt zbudowaliśmy za pomocą dedykowanego mikroprocesorom środowiska PlatformIO instalowanego jako wtyczka do Visual Studio Code.

### Instalacja
Żeby uruchomić PlatformIO IDE trzeba zainstalować:
- [pythona](https://www.python.org/downloads/) w wersji co najmniej 3.6
- środowisko [VS Code](https://code.visualstudio.com/download)
- wtyczkę [Platform IO](https://docs.platformio.org/en/latest/integration/ide/vscode.html#ide-vscode)


<!-- dodanie płytki -->
<!-- sterowniki -->

### Programowanie płytki

## Prototyp
<!-- schemat układu -->
<!-- zdjęcia -->

## Testy i kalibracja
<!-- kacprowa opowieść -->
<!-- zebrane dane -->

## Interfejs web
![](images/image5.png)