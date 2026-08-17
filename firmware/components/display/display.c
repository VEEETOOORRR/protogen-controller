#include "display.h"

static spi_device_handle_t spi_handle;

uint8_t framebuffer[NUM_MODULES * 8] = {0};

void spi_conf(){

    // Configuração do barramento SPI
    spi_bus_config_t busfcg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = NUM_MODULES * 2
    };

    // Configuração específica para o MAX7219
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10000000, // 10 MHz
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 7
    };

    // Inicializa barramento
    spi_bus_initialize(SPI2_HOST, &busfcg, SPI_DMA_CH_AUTO); 

    // Adiciona dispositivo ao barramento
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);

    display_write_all(MAX7219_REG_DISPLAY_TEST, 0x00); // Desativa modo de teste (se estivesse ativo)
    display_write_all(MAX7219_REG_SCAN_LIMIT, 0x07);   // Configura para escanear todas as 8 linhas/colunas
    display_write_all(MAX7219_REG_DECODE_MODE, 0x00);  // Modo Matriz de Led crua (sem decodificação BCD de 7 segmentos)
    display_write_all(MAX7219_REG_INTENSITY, DISPLAY_INTENSITY);    // Brilho inicial baixo (0x00 a 0x0F). Bom para poupar energia em testes!

    // Limpa todas as linhas/colunas (apaga tudo antes de ligar a tela para evitar flashes estáticos)
    for (int i = 1; i <= 8; i++) {
        display_write_all(i, 0x00);
    }

    display_write_all(MAX7219_REG_SHUTDOWN, 0x01);    // Acorda o chip (Modo normal de operação)

}

// Função para escrever em todos os módulos do display
void display_write_all(uint8_t reg, uint8_t data){
    
    uint8_t *tx_buf = malloc(NUM_MODULES * 2);

    for (int i = 0; i < NUM_MODULES * 2; i += 2){
        tx_buf[i] = reg;
        tx_buf[i+1] = data;
    }

    spi_transaction_t t;

    memset(&t, 0, sizeof(t)); // Limpa t

    t.length = NUM_MODULES * 2 * 8;
    t.tx_buffer = tx_buf;

    spi_device_transmit(spi_handle, &t);

    free(tx_buf);
}

void display_render(){
    // O MAX7219 tem 8 registradores de dígitos (linhas) de 0x01 a 0x08
    for (uint8_t line = 0; line < 8; line++) {
        uint8_t tx_buf[NUM_MODULES * 2];
        
        // Monta o pacote SPI para essa linha específica em todas as matrizes
        for (int i = 0; i < NUM_MODULES; i++) {
            // Endereço do registrador da linha (0x01 a 0x08)
            tx_buf[i * 2]     = MAX7219_REG_DIGIT0 + line; 
            
            // Pega o byte correspondente a essa linha no framebuffer
            // Aqui fazemos o mapeamento linear da memória
            tx_buf[i * 2 + 1] = framebuffer[line + (i * 8)]; 
        }

        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = NUM_MODULES * 2 * 8;
        t.tx_buffer = tx_buf;

        spi_device_transmit(spi_handle, &t);
    }
}

