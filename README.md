# README - Projeto de Controle com Joystick e Display OLED

## Descrição
Este projeto utiliza um joystick analógico para controlar um quadrado em um display OLED SSD1306, juntamente com LEDs controlados via PWM. Foi desenvolvido para a placa Raspberry Pi Pico e faz uso das bibliotecas da RP2040 para manipulação de GPIO, ADC, I2C e PWM.

## Componentes Utilizados
- **Microcontrolador:** Raspberry Pi Pico
- **Display OLED:** SSD1306 (conectado via I2C)
- **Joystick:** Módulo analógico com botão
- **LEDs:** RGB (controlados via PWM)
- **Botão:** Botão extra para interação

## Esquema de Ligação
### **Pinos Utilizados**
- **Joystick:**
  - VRX: GPIO 26 (ADC0)
  - VRY: GPIO 27 (ADC1)
  - Botão do Joystick: GPIO 22
- **Botão Extra:** GPIO 5
- **LEDs:**
  - LED Verde: GPIO 11
  - LED Azul: GPIO 12 (PWM)
  - LED Vermelho: GPIO 13 (PWM)
- **Display OLED (SSD1306):**
  - SDA: GPIO 14
  - SCL: GPIO 15

## Funcionamento
### **Movimentação do Quadrado**
O joystick controla a posição de um quadrado 8x8 no display OLED. Os valores analógicos do joystick são lidos via ADC e mapeados para as coordenadas da tela (128x64 pixels).

### **Controle de LEDs**
- O LED vermelho responde ao eixo X do joystick.
- O LED azul responde ao eixo Y do joystick.
- O LED verde acende quando o botão do joystick é pressionado.

### **Funções Extras**
- Quando o botão do joystick é pressionado, exibe "SEGUNDA FASE" e "CAIO BRUNO" no display.
- Se o botão A for pressionado, os LEDs não respondem ao joystick.

### **Nova Funcionalidade**
- Implementada animação do quadrado ao se mover pelo display.
- Adicionada uma opção de calibração do joystick ao iniciar o sistema.
- Melhorado o mapeamento dos eixos X e Y para suavizar a resposta.

## Dependências
- **SDK do Raspberry Pi Pico**
- **Bibliotecas:**
  - `hardware/adc.h`
  - `hardware/pwm.h`
  - `hardware/i2c.h`
  - `ssd1306.h` (controlador do display OLED)
  - `font.h` (fonte para textos no display)

## Compilação e Execução
1. Configure o ambiente de desenvolvimento para o Raspberry Pi Pico (SDK do Pico).
2. Compile o código usando CMake:
   ```sh
   mkdir build && cd build
   cmake ..
   make
   ```
3. Envie o binário gerado para o Raspberry Pi Pico.
4. Conecte o Pico a uma fonte de alimentação e observe o comportamento do display e dos LEDs.

## Autor
Desenvolvido por **Caio Bruno**

