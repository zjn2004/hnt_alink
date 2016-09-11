#ifndef _USER_SMARTPLUG_H
#define _USER_SMARTPLUG_H

#define GPIO_VALUE_0 0
#define GPIO_VALUE_1 1

/* LED */
#define SMARTPLUG_POWER_LED_GPIO     12

#define SMARTPLUG_WIFI_LED_GPIO      13


/* KEY */
#define SMARTPLUG_KEY_NUM          2

#define SMARTPLUG_KEY1_IO_MUX       PERIPHS_IO_MUX_MTMS_U
#define SMARTPLUG_KEY1_IO_GPIO      14
#define SMARTPLUG_KEY1_IO_FUNC      FUNC_GPIO14

#define SMARTPLUG_KEY2_IO_MUX       PERIPHS_IO_MUX_GPIO4_U
#define SMARTPLUG_KEY2_IO_GPIO      4
#define SMARTPLUG_KEY2_IO_FUNC      FUNC_GPIO4

/* CONTROL */
#define SMARTPLUG_CTRL_IO_MUX     PERIPHS_IO_MUX_MTDO_U
#define SMARTPLUG_CTRL_IO_GPIO    15
#define SMARTPLUG_CTRL_IO_FUNC    FUNC_GPIO15

/* PATH */
#define SMARTPLUG_POWER                   "OnOff_Power"

/* CUSTOM INFO */
/* device info */
#define DEV_NAME "XIAOMENG_ALINK"
#define DEV_CATEGORY "LIVING"
#define DEV_TYPE "OUTLET"
#define DEV_BRAND "BOOMSENSE"
#define DEV_MANUFACTURE "SEAING"

#define DEV_MODEL "SEAING_LIVING_OUTLET_XIAOMENG"

#define ALINK_KEY "HXv2lHVfvwDLyvWWuzEa"
#define ALINK_SECRET "a1XcvPOJGzdmqVhQzG6fEzloT4zqlFdARArlyzVy"
#define ALINK_TPSK "OC3ICf8NIEM5ZUIl1gz2IeMnPTZG0D6oFErTIzPppN0="


/*sandbox key/secret*/
#define ALINK_KEY_SANDBOX "2tcHGVrhB96xMrUzN5oc"
#define ALINK_SECRET_SANDBOX "M44yDCSQQvZ7v98Fuismj7t1Cf0zRTPlAZm8k0Zb"
#define ALINK_TPSK_SANDBOX "N8G59AP+4TPp/hpFjLvSWKRlPU6b3SESHbLfEX8yruA="


/*设备硬件信息:系统上电后读取的硬件/固件信息,此处为演示需要,直接定义为宏.产品对接时,需要调用自身接口获取*/

#define DEV_SN "1234567890"
#define DEV_VERSION "2.0.4"

typedef void (*hnt_power_led_func)(unsigned char);
#define POWER_LED_ON             1
#define POWER_LED_OFF            0

#endif /*_USER_SMARTPLUG_H */
