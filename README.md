# Controle de Acesso com Arduino e ESP32

Este projeto consiste em um sistema de controle de acesso desenvolvido com Arduino e ESP32, integrado à API do WhatsApp CallMeBot para notificação automática via mensagem quando uma chave é retirada ou devolvida ao local.

![Exemplo de Mensagem](msg.png)

## Funcionalidades

- Notificação automática via WhatsApp ao retirar ou devolver uma chave.
- Monitoramento em tempo real do estado das chaves.

## Hardware Necessário

- Placa Arduino (exemplo: Arduino Uno)
- Placa ESP32 (ou módulo ESP8266)
- Módulo de relé para controle dos dispositivos de segurança
- Smartphone com WhatsApp para recebimento das notificações

```cpp
# define WHATSAPP_BOT_TOKEN "seu_token"
# define WHATSAPP_DESTINO "seu_numero_de_telefone"
