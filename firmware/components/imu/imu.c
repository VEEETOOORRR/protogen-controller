#include "imu.h"

static const char *TAG = "IMU";
static i2c_master_dev_handle_t imu_handle;

// Função auxiliar para escrever um byte em um registrador do LSM6DS3
static void lsm6ds3_write_reg(uint8_t reg, uint8_t data) {
    uint8_t write_buf[2] = {reg, data};
    i2c_master_transmit(imu_handle, write_buf, sizeof(write_buf), -1);
}

// A Task que fará a leitura e o filtro complementar
static void imu_task(void *arg) {

    system_event_t event = {.type = EVENT_IMU_CENTERED, .payload = 0};

    uint8_t reg_addr = 0x22; 
    uint8_t data[12];
    
    float pitch = 0.0f;
    float roll = 0.0f;
    float dt = 0.02f; // 20ms (50Hz)

    const float gyro_scale = 70.0f / 1000.0f; 
    
    bool is_tilted = false;

    // --- CALIBRAÇÃO DO GIROSCÓPIO ---
    float gyro_x_offset = 0.0f;
    float gyro_y_offset = 0.0f;
    int calib_samples = 100;
    
    ESP_LOGI(TAG, "Calibrando IMU. Mantenha a placa parada...");
    for (int i = 0; i < calib_samples; i++) {
        if (i2c_master_transmit_receive(imu_handle, &reg_addr, 1, data, 12, -1) == ESP_OK) {
            int16_t raw_gx = (data[1] << 8) | data[0];
            int16_t raw_gy = (data[3] << 8) | data[2];
            gyro_x_offset += raw_gx * gyro_scale;
            gyro_y_offset += raw_gy * gyro_scale;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    gyro_x_offset /= calib_samples;
    gyro_y_offset /= calib_samples;
    ESP_LOGI(TAG, "Calibração concluída!");
    // ---------------------------------

    for (;;) {
        esp_err_t ret = i2c_master_transmit_receive(imu_handle, &reg_addr, 1, data, 12, -1);
        
        if (ret == ESP_OK) {
            int16_t raw_gx = (data[1] << 8) | data[0];
            int16_t raw_gy = (data[3] << 8) | data[2];
            
            int16_t raw_ax = (data[7] << 8) | data[6];
            int16_t raw_ay = (data[9] << 8) | data[8];
            int16_t raw_az = (data[11] << 8) | data[10];

            // 1. Converte giroscópio e remove o erro (Bias) de calibração
            float gx_dps = (raw_gx * gyro_scale) - gyro_x_offset;
            float gy_dps = (raw_gy * gyro_scale) - gyro_y_offset;

            // 2. Acelerômetro
            float accel_pitch = atan2(-raw_ax, sqrt(raw_ay * raw_ay + raw_az * raw_az)) * 180.0 / M_PI;
            float accel_roll = atan2(raw_ay, raw_az) * 180.0 / M_PI;

            // 3. FILTRO COMPLEMENTAR
            pitch = 0.96f * (pitch + gy_dps * dt) + 0.04f * accel_pitch;
            roll  = 0.96f * (roll  + gx_dps * dt) + 0.04f * accel_roll;

            // 4. Lógica de Interação e Tratamento de Eventos
            if (!is_tilted) {
                if (roll > 30.0f) {
                    is_tilted = true;
                    ESP_LOGI(TAG, "Cabeça inclinada para a Direita.");
                    event.type = EVENT_IMU_TILT_RIGHT;
                    control_send_event(&event);
                } else if (roll < -30.0f) {
                    is_tilted = true;
                    ESP_LOGI(TAG, "Cabeça inclinada para a Esquerda.");
                    event.type = EVENT_IMU_TILT_LEFT;
                    control_send_event(&event);
                }
            } 
            else if (roll > -15.0f && roll < 15.0f && is_tilted) {
                is_tilted = false;
                ESP_LOGI(TAG, "Cabeça reta.");
                event.type = EVENT_IMU_CENTERED;
                control_send_event(&event);
            }

        } else {
            ESP_LOGE(TAG, "Erro na leitura do I2C");
        }
        
        // Mantém ESTRITAMENTE a frequência de 50Hz (20ms)
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void imu_init(void) {
    ESP_LOGI(TAG, "Inicializando I2C Master...");

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = SCL_PIN,
        .sda_io_num = SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LSM6DS3_ADDR,
        .scl_speed_hz = 400000, // I2C Fast Mode (400kHz)
    };

    i2c_master_bus_add_device(bus_handle, &dev_cfg, &imu_handle);

    // Acordando o LSM6DS3
    // CTRL1_XL (0x10): Acelerômetro ligado em 104Hz, fundo de escala +/- 2g
    lsm6ds3_write_reg(0x10, 0x40);
    
    // CTRL2_G (0x11): Giroscópio ligado em 104Hz, fundo de escala 2000 dps
    lsm6ds3_write_reg(0x11, 0x4C);

    // Cria a task do IMU com prioridade média (acima das tarefas básicas, mas não critica)
    xTaskCreate(imu_task, "imu_task", 4096, NULL, 5, NULL);
}