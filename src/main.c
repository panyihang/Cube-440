#include "pico/pdm_microphone.h"
#include "blink.pio.h"
#include "usb_microphone.h"
#include "hardware/clocks.h"
#include "st7789.h"
#include "hardware/gpio.h"

const struct st7789_config lcd_config = {
    .spi = PICO_DEFAULT_SPI_INSTANCE,
    .gpio_din = 19,
    .gpio_clk = 18,
    .gpio_cs = 20,
    .gpio_dc = 22,
    .gpio_rst = 21,
    .gpio_bl = 9,
};

const int lcd_width = 81;
const int lcd_height = 163;

const struct pdm_microphone_config config = {
    .gpio_data = 2,
    .gpio_clk = 3,
    .pio = pio0,
    .pio_sm = 0,
    .sample_rate = SAMPLE_RATE,
    .sample_buffer_size = SAMPLE_BUFFER_SIZE,
};

uint16_t sample_buffer[SAMPLE_BUFFER_SIZE];

void on_pdm_samples_ready();
void on_usb_microphone_tx_ready();


void gpio_callback(uint gpio, uint32_t events) {
  st7789_fill(0x0000);
}


void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq)
{
  blink_program_init(pio, sm, offset, pin);
  pio_sm_set_enabled(pio, sm, true);

  printf("Blinking pin %d at %d Hz\n", pin, freq);
  pio->txf[sm] = clock_get_hz(clk_sys) / (2 * freq);
}

int main(void)
{
  pdm_microphone_init(&config);
  pdm_microphone_set_samples_ready_handler(on_pdm_samples_ready);
  pdm_microphone_start();

  usb_microphone_init();
  usb_microphone_set_tx_ready_handler(on_usb_microphone_tx_ready);

  st7789_init(&lcd_config, lcd_width, lcd_height);
  st7789_fill(0xffff);


  gpio_init(4);
  gpio_init(5);
  gpio_pull_up(4);
  gpio_pull_up(5);
  gpio_set_dir(4, GPIO_IN);
  gpio_set_dir(5, GPIO_IN);
  gpio_set_irq_enabled_with_callback(4,  GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
  
  
#if 1
  PIO pio = pio0;
  uint offset = pio_add_program(pio, &blink_program);
  printf("Loaded program at %d\n", offset);
  blink_pin_forever(pio, 1, offset, 11, 5);
#endif

  while (1)
  {
    usb_microphone_task();
  }

  return 0;
}

void on_pdm_samples_ready()
{
  pdm_microphone_read(sample_buffer, SAMPLE_BUFFER_SIZE);
}

void on_usb_microphone_tx_ready()
{
  usb_microphone_write(sample_buffer, sizeof(sample_buffer));
}