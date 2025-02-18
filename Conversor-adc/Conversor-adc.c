#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"     
#include "hardware/pwm.h"   
#include "hardware/i2c.h"   
#include "lib/ssd1306.h"   
#include "lib/font.h"

#define LED_v 11
#define LED_a 12
#define LED_r 13
#define button_A 5
#define VRX 26
#define VRY 27
#define button_joy 22

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
uint pwm_wrap = 4096;   

ssd1306_t ssd; 
#define WIDTH 128 //128
#define HEIGHT 64 //64
#define SQUARE_SIZE 8

//Variaveis para tratar os botões
const uint32_t debounce_time_ms = 200;
volatile bool button_a_pressed = false;
volatile bool button_joy_pressed = false;
absolute_time_t last_interrupt_time = 0;


//Inicializa o LED RGB com PWM
uint pwm_initialization_pin(uint pin, uint wrap){
    gpio_set_function(pin, GPIO_FUNC_PWM);

    uint slice = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice, wrap);
    pwm_set_enabled(slice, wrap);
    return slice;
}

//Inicializa os pinos
void initialization(){
    //Joystick
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);

    //LED verde
    gpio_init(LED_v);
    gpio_set_dir(LED_v, GPIO_OUT);
    gpio_put(LED_v, 0);

    //Botão A
    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN);
    gpio_pull_up(button_A);

    //Botão do joystick
    gpio_init(button_joy);
    gpio_set_dir(button_joy, GPIO_IN);
    gpio_pull_up(button_joy);

    //LED RGB com o wrap
    uint pwm_slice = pwm_initialization_pin(LED_r, pwm_wrap);
    uint pwm_slice2 = pwm_initialization_pin(LED_a, pwm_wrap);

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

}

int map_value(int value, int in_min, int in_max, int out_min, int out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Função de interrupção
void gpio_irq_handler(uint gpio, uint32_t events){
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_interrupt_time, now) < debounce_time_ms * 1000)
        return;
    last_interrupt_time = now;

    if(gpio == button_A){
        button_a_pressed = !button_a_pressed;
    }else if (gpio == button_joy){
        button_joy_pressed = !button_joy_pressed;
        gpio_put(LED_v, !gpio_get(LED_v));
    }

}


int main()
{
    initialization(); //Inicializa os pinos

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    int square_size = 8;
    int square_x = WIDTH / 2 - SQUARE_SIZE / 2;
    int square_y = HEIGHT / 2 - SQUARE_SIZE / 2;

    uint32_t last_print_time = 0; 
    bool cor = true;

    //Chamadas de interrupções
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(button_joy, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true) {
        adc_select_input(1);
        uint16_t vrx_value = adc_read();
        adc_select_input(0);
        uint16_t vry_value = adc_read();

        //Converte os valores da entrada ADC
        square_x = map_value(vry_value, 0, 4095, HEIGHT - SQUARE_SIZE, 0);
        square_y = map_value(vrx_value, 0, 4095, 0 , WIDTH - SQUARE_SIZE);

        ssd1306_fill(&ssd, false);

        if (button_a_pressed == false){
            //Para controlar o PWM do LED vermelho no eixo X
            if (vrx_value < 1810){
                float factor = 1.0 - ((float)vrx_value / 2010.0);
                uint pwm_value = 2010 + (pwm_wrap - 2010) * pow(factor, 2.5);
                pwm_set_gpio_level(LED_r, pwm_value);
            } else if (vrx_value > 1890) {
                pwm_set_gpio_level(LED_r, vrx_value);
            }else {
                pwm_set_gpio_level(LED_r, 0);
            }
            
            //Para controlar o PWM do LED azul no eixo y
            if (vry_value < 2010){
                float factor = 1.0 - ((float)vry_value / 2010.0);
                uint pwm_value = 2010 + (pwm_wrap - 2010) * pow(factor, 2.5);
                pwm_set_gpio_level(LED_a, pwm_value);
            } else if (vry_value > 2090) {
                pwm_set_gpio_level(LED_a, vry_value);
            }else {
                pwm_set_gpio_level(LED_a, 0);
            }
        }

        //Se o botão do joystick for pressionado
        if(button_joy_pressed){
            ssd1306_draw_string(&ssd, "SEGUNDA FASE", 16, 6); // Escreve na parte de cima
            ssd1306_draw_string(&ssd, "CAIO BRUNO", 20, 55); // Escreve na parte de baixo
        }else {
            ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
        }
        
        ssd1306_rect(&ssd, square_x, square_y, SQUARE_SIZE, SQUARE_SIZE, true, true);//Deseho um quadrado no Display que vai seguir o Joystick
        ssd1306_send_data(&ssd); // Atualiza o display
        
        sleep_ms(100);
    }
}

