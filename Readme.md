""# P_Pir_Relay

Un gestionnaire d'éclairage automatique et intelligent pour ESP8266/ESP32, combinant détection de présence, planification astronomique et une interface de configuration web embarquée.

---

## 📋 Présentation

**P_Pir_Relay** est une solution firmware robuste et modulaire dédiée à l'automatisation de l'éclairage. Conçu pour être hautement configurable, le système gère les entrées matérielles (PIR, capteurs), calcule les horaires d'éclairage selon la position solaire et offre une gestion locale via une interface web intégrée au microcontrôleur.

---

## 📐 Architecture du Système

Le diagramme ci-dessous détaille la structure logicielle actuelle, la séparation des composants et le rôle du build PlatformIO pour le déploiement des assets web et du firmware :

```mermaid
flowchart TD

subgraph group_firmware["Firmware"]
  node_main["Device lifecycle<br/>application entry point<br/>[main.cpp]"]
  node_config["Firmware settings<br/>shared configuration<br/>[Config.h]"]
  node_hardware{{"Hardware I/O<br/>GPIO boundary<br/>[Hardware.cpp]"}}
  node_network["Network service<br/>connectivity and HTTP<br/>[Network.cpp]"]
  node_storage[("Persistent storage<br/>persistence API<br/>[Storage.cpp]")]
  node_astro["Astronomical timing<br/>domain service<br/>[Astro.cpp]"]
end

subgraph group_web["On-device web UI"]
  node_index["Status page<br/>static UI asset<br/>[index.html]"]
  node_setup["Setup page<br/>static UI asset<br/>[setup.html]"]
  node_wait["Setup progress page<br/>static UI asset<br/>[wait.html]"]
  node_success["Setup completion page<br/>static UI asset<br/>[success.html]"]
end

subgraph group_deployment["Build & deployment"]
  node_platformio["PlatformIO project<br/>build definition<br/>[platformio.ini]"]
end

node_platformio -->|"builds firmware"| node_main
node_platformio -->|"deploys filesystem assets"| node_index
node_platformio -->|"deploys filesystem assets"| node_setup
node_platformio -->|"deploys filesystem assets"| node_wait
node_platformio -->|"deploys filesystem assets"| node_success
node_main -->|"reads motion and controls relay"| node_hardware
node_main -->|"runs device web service"| node_network
node_main -->|"loads and updates settings"| node_storage
node_main -->|"uses timing conditions"| node_astro
node_config -.->|"I/O settings"| node_hardware
node_config -.->|"storage settings"| node_storage
node_network -->|"persists setup input"| node_storage
node_network -->|"serves"| node_index
node_network -->|"serves"| node_setup
node_network -->|"serves"| node_wait
node_network -->|"serves"| node_success

click node_platformio "[https://github.com/kingfrl/p_pir_relay/blob/main/platformio.ini](https://github.com/kingfrl/p_pir_relay/blob/main/platformio.ini)"
click node_main "[https://github.com/kingfrl/p_pir_relay/blob/main/src/main.cpp](https://github.com/kingfrl/p_pir_relay/blob/main/src/main.cpp)"
click node_config "[https://github.com/kingfrl/p_pir_relay/blob/main/src/Config.h](https://github.com/kingfrl/p_pir_relay/blob/main/src/Config.h)"
click node_hardware "[https://github.com/kingfrl/p_pir_relay/blob/main/src/Hardware.cpp](https://github.com/kingfrl/p_pir_relay/blob/main/src/Hardware.cpp)"
click node_network "[https://github.com/kingfrl/p_pir_relay/blob/main/src/Network.cpp](https://github.com/kingfrl/p_pir_relay/blob/main/src/Network.cpp)"
click node_storage "[https://github.com/kingfrl/p_pir_relay/blob/main/src/Storage.cpp](https://github.com/kingfrl/p_pir_relay/blob/main/src/Storage.cpp)"
click node_astro "[https://github.com/kingfrl/p_pir_relay/blob/main/src/Astro.cpp](https://github.com/kingfrl/p_pir_relay/blob/main/src/Astro.cpp)"
click node_index "[https://github.com/kingfrl/p_pir_relay/blob/main/data/index.html](https://github.com/kingfrl/p_pir_relay/blob/main/data/index.html)"
click node_setup "[https://github.com/kingfrl/p_pir_relay/blob/main/data/setup.html](https://github.com/kingfrl/p_pir_relay/blob/main/data/setup.html)"
click node_wait "[https://github.com/kingfrl/p_pir_relay/blob/main/data/wait.html](https://github.com/kingfrl/p_pir_relay/blob/main/data/wait.html)"
click node_success "[https://github.com/kingfrl/p_pir_relay/blob/main/data/success.html](https://github.com/kingfrl/p_pir_relay/blob/main/data/success.html)"

classDef toneNeutral fill:#f8fafc,stroke:#334155,stroke-width:1.5px,color:#0f172a
classDef toneBlue fill:#dbeafe,stroke:#2563eb,stroke-width:1.5px,color:#172554
classDef toneAmber fill:#fef3c7,stroke:#d97706,stroke-width:1.5px,color:#78350f
classDef toneMint fill:#dcfce7,stroke:#16a34a,stroke-width:1.5px,color:#14532d
class node_main,node_config,node_hardware,node_network,node_storage,node_astro toneBlue
class node_index,node_setup,node_wait,node_success toneAmber
class node_platformio toneMint