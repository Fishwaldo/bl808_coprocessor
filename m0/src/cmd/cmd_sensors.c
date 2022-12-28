#include <bflb_adc.h>
#include "cmd_sensors.h"

int cmd_temp(int argc, char **argv) {
    uint16_t i = 0;
    float average_filter = 0.0;

    struct bflb_device_s *adc = bflb_device_get_by_name("adc");

    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_2P0V;

    struct bflb_adc_channel_s chan;

    chan.pos_chan = ADC_CHANNEL_TSEN_P;
    chan.neg_chan = ADC_CHANNEL_GND;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, &chan, 1);
    bflb_adc_tsen_init(adc, ADC_TSEN_MOD_INTERNAL_DIODE);

    for (i = 0; i < 50; i++) {
        average_filter += bflb_adc_tsen_get_temp(adc);
        bflb_mtimer_delay_ms(10);
    }
    printf("temp = %d\r\n", (uint32_t)(average_filter / 50.0));
    return 0;
}

int cmd_voltage(int argc, char **argv) {
    struct bflb_device_s *adc = bflb_device_get_by_name("adc");
 
    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    struct bflb_adc_channel_s chan;

    chan.pos_chan = ADC_CHANNEL_VABT_HALF;
    chan.neg_chan = ADC_CHANNEL_GND;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, &chan, 1);
    bflb_adc_vbat_enable(adc);

    struct bflb_adc_result_s result;

    bflb_adc_start_conversion(adc);
    while (bflb_adc_get_count(adc) == 0) {
        bflb_mtimer_delay_ms(1);
    }
    uint32_t raw_data = bflb_adc_read_raw(adc);

    bflb_adc_parse_result(adc, &raw_data, &result, 1);
    printf("vBat = %d mV\r\n", (uint32_t)(result.millivolt * 2));
    bflb_adc_stop_conversion(adc);
    bflb_adc_vbat_disable(adc);
    return 0;
}

