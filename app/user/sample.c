/*
* Copyright (c) 2014-2015 Alibaba Group. All rights reserved.
*
* Alibaba Group retains all right, title and interest (including all
* intellectual property rights) in and to this computer program, which is
* protected by applicable intellectual property laws.  Unless you have
* obtained a separate written license from Alibaba Group., you are not
* authorized to utilize all or a part of this computer program for any
* purpose (including reproduction, distribution, modification, and
* compilation into object code), and you must immediately destroy or
* return to Alibaba Group all copies of this computer program.  If you
* are licensed by Alibaba Group, your rights to utilize this computer
* program are limited by the terms of that license.  To obtain a license,
* please contact Alibaba Group.
*
* This computer program contains trade secrets owned by Alibaba Group.
* and, unless unauthorized by Alibaba Group in writing, you agree to
* maintain the confidentiality of this computer program and related
* information and to not disclose this computer program and related
* information to any other person or entity.
*
* THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
* Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
* INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
* PURPOSE, TITLE, AND NONINFRINGEMENT.
*/
#include "c_types.h"
#include "alink_export.h"
#include "alink_json.h"
#include <stdio.h>
#include <string.h>
#include "alink_export_rawdata.h"
#include "esp_common.h"
#include "user_config.h"
#include "hnt_interface.h"
#include "ledctl.h" 

#if USER_UART_CTRL_DEV_EN
#include "user_uart.h" // user uart handler head
#endif

deviceParameterTable_t DeviceCustomParamTable = {NULL, 0};
customInfo_t *DeviceCustomInfo = NULL;

#define wsf_deb  os_printf
#define wsf_err os_printf
//#define PASS_THROUGH 
#if 0
/* �豸��Ϣ��������ҳע����Ϣ�����ĵ��ӱ����¶�Ӧ��Ϣ */
/* device info */
#define DEV_NAME "ALINKTEST"
#define DEV_CATEGORY "LIVING"
#define DEV_TYPE "LIGHT"
#ifdef PASS_THROUGH
#define DEV_MODEL "ALINKTEST_LIVING_LIGHT_SMARTLED_LUA"
#define ALINK_KEY "bIjq3G1NcgjSfF9uSeK2"
#define ALINK_SECRET "W6tXrtzgQHGZqksvJLMdCPArmkecBAdcr2F5tjuF"
#else
//#define DEV_MODEL "ALINKTEST_LIVING_LIGHT_SMARTLED"
//#define ALINK_KEY "ljB6vqoLzmP8fGkE6pon"
//#define ALINK_SECRET "YJJZjytOCXDhtQqip4EjWbhR95zTgI92RVjzjyZF"
#define DEV_MODEL "SEAING_LIVING_OUTLET_XIAOMENG"
#define ALINK_KEY "HXv2lHVfvwDLyvWWuzEa"
#define ALINK_SECRET "a1XcvPOJGzdmqVhQzG6fEzloT4zqlFdARArlyzVy"

#endif
#define DEV_MANUFACTURE "ALINKTEST"
/*sandbox key/secret*/
#define ALINK_KEY_SANDBOX "dpZZEpm9eBfqzK7yVeLq"
#define ALINK_SECRET_SANDBOX "THnfRRsU5vu6g6m9X6uFyAjUWflgZ0iyGjdEneKm"
/*�豸Ӳ����Ϣ:ϵͳ�ϵ���ȡ��Ӳ��/�̼���Ϣ,�˴�Ϊ��ʾ��Ҫ,ֱ�Ӷ���Ϊ��.��Ʒ�Խ�ʱ,��Ҫ��������ӿڻ�ȡ*/
#define DEV_SN "1234567890"
#define DEV_VERSION "1.0.0;APP2.0;OTA1.0"
#define DEV_MAC "19:FE:34:A2:C7:1A"	//"AA:CC:CC:CA:CA:01" // need get from device
#define DEV_CHIPID "3D0044000F47333139373030"	// need get from device
#endif
/*alink-sdk ��Ϣ */

extern void alink_sleep(int);
/*do your job here*/
/*������һ��������豸,��5���豸���Զ�Ӧ��ֵ���浽ȫ�ֱ���,��ʵ���豸��Ҫȥ����ʵ��ҵ������Щ����ֵ */

VIRTUAL_DEV virtual_device;// = {0x01, 0x30, 0x50, 0, 0x01};

//const char *main_dev_params =
  //  "{\"attrSet\": [ \"OnOff_Power\", \"Color_Temperature\", \"Light_Brightness\", \"TimeDelay_PowerOff\", \"WorkMode_MasterLight\"], \"OnOff_Power\": { \"value\": \"%d\" }, \"Color_Temperature\": { \"value\": \"%d\" }, \"Light_Brightness\": { \"value\": \"%d\" }, \"TimeDelay_PowerOff\": { \"value\": \"%d\"}, \"WorkMode_MasterLight\": { \"value\": \"%d\"}}";
char *device_attr[5] = { "OnOff_Power", "Color_Temperature", "Light_Brightness",
	"TimeDelay_PowerOff", "WorkMode_MasterLight"
};   // this is a demo json package data, real device need to update this package

const char *main_dev_params =
    "{\"OnOff_Power\": { \"value\": \"%d\" }, \"Color_Temperature\": { \"value\": \"%d\" }, \"Light_Brightness\": { \"value\": \"%d\" }, \"TimeDelay_PowerOff\": { \"value\": \"%d\"}, \"WorkMode_MasterLight\": { \"value\": \"%d\"}}";

char device_status_change = 1;
/*�豸�ϱ�����,��Ҫ�ͻ����ݾ���ҵ��ȥʵ��*/

#define buffer_size 512
static int ICACHE_FLASH_ATTR alink_device_post_data(alink_down_cmd_ptr down_cmd)
{
	alink_up_cmd up_cmd;
	int ret = ALINK_ERR;
	char *buffer = NULL;
    deviceParameter_t *deviceParam;  
    int i;
    int len = 0;    
    char value_char[32];
    
    printf("alink_device_post_data\n");

	wsf_deb("##[%s][%s|%d]Malloc %u. Available memory:%d.\n", __FILE__, __FUNCTION__, __LINE__,
		buffer_size, system_get_free_heap_size());

	buffer = (char *)malloc(buffer_size);
	if (buffer == NULL)
		return -1;

	memset(buffer, 0, buffer_size);

	len = sprintf(buffer, "{");
    
    for (i = 0, deviceParam = DeviceCustomParamTable.table; 
         i < DeviceCustomParamTable.tableSize;
         i++, deviceParam++) {        
            if(deviceParam->getParameterFunc){
                deviceParam->getParameterFunc(deviceParam->paramName, value_char);
                len += sprintf(buffer+len, "\"%s\": { \"value\": \"%s\" },",
                    deviceParam->paramName,value_char);                    
                }
    }
    len = len-1;//strip last ","
    len += sprintf(buffer+len, "}");
    
    wsf_deb("post data:%s\n",buffer);
    
	up_cmd.param = buffer;
	if (down_cmd != NULL) {
		up_cmd.target = down_cmd->account;
		up_cmd.resp_id = down_cmd->id;
	} else {
		up_cmd.target = NULL;
		up_cmd.resp_id = -1;
	}
	ret = alink_post_device_data(&up_cmd);

	if (ret == ALINK_ERR) {
		wsf_err("post failed!\n");
		alink_sleep(1000);
	} else {
		wsf_deb("dev post data success!\n");
		device_status_change = 0;
	}

	if (buffer)
		free(buffer);
    
	wsf_deb("##[%s][%s][%d]  Free |Aviable Memory|%5d| \n", __FILE__, __FUNCTION__, __LINE__,
		system_get_free_heap_size());

	stack_free_size();
    
	return ret;
}

/* do your job end */

int sample_running = ALINK_TRUE;

/*get json cmd from server �������·�����,��Ҫ�豸�˸��ݾ���ҵ���趨ȥ��������*/
int ICACHE_FLASH_ATTR main_dev_set_device_status_callback(alink_down_cmd_ptr down_cmd)
{

	json_value *jptr;
	json_value *jstr;
	json_value *jstr_value;
	int  i = 0;
	char *value_str = NULL;
    deviceParameter_t *deviceParam;  
    int ret = 0;

	wsf_deb("%s %d \n",__FUNCTION__,__LINE__);
	wsf_deb("%s %d %s\n",down_cmd->uuid,down_cmd->method, down_cmd->param);

	jptr = json_parse(down_cmd->param, strlen(down_cmd->param));
#if USER_UART_CTRL_DEV_EN
	for (i = 0,deviceParam = DeviceCustomParamTable.table; 
        i < DeviceCustomParamTable.tableSize; 
        i++, deviceParam++) 
	{
		jstr = json_object_object_get_e(jptr, deviceParam->paramName);
		jstr_value = json_object_object_get_e(jstr, "value");
		value_str = json_object_to_json_string_e(jstr_value);
        
        wsf_deb("%s\n",deviceParam->paramName);
		
		if (value_str) {
            wsf_deb("%s %s\n",deviceParam->paramName,value_str);
            
            if(deviceParam->setParameterFunc)
                ret = deviceParam->setParameterFunc(deviceParam->paramName, value_str);
		}
	}
#endif

	json_value_free(jptr);
	device_status_change = 1;   // event send current real device status to the alink server

	return ret;		// alink_device_post_data(down_cmd);
	/* do your job end! */
}

/*��������ѯ�豸״̬,��Ҫ�豸�ϱ�״̬*/
int ICACHE_FLASH_ATTR main_dev_get_device_status_callback(alink_down_cmd_ptr down_cmd)
{
	wsf_deb("%s %d \n", __FUNCTION__, __LINE__);
	wsf_deb("%s %d\n%s\n", down_cmd->uuid, down_cmd->method, down_cmd->param);
	device_status_change = 1;

	return 0;		//alink_device_post_data(down_cmd);
}

/* ���ݲ�ͬϵͳ��ӡʣ���ڴ�,����ƽ̨���� */
int ICACHE_FLASH_ATTR print_mem_callback(void *a, void *b)
{
	int ret = 0;
	ret = system_get_free_heap_size();
	os_printf("heap_size %d\n", ret);
	return ret;
}

/*alink-sdk ״̬��ѯ�ص�����*/
int ICACHE_FLASH_ATTR alink_handler_systemstates_callback(void *dev_mac, void *sys_state)
{
	char uuid[33] = { 0 };
	char *mac = (char *)dev_mac;
	enum ALINK_STATUS *state = (enum ALINK_STATUS *)sys_state;
	switch (*state) {
	case ALINK_STATUS_INITED:
		sprintf(uuid, "%s", alink_get_uuid(NULL));
		wsf_deb("ALINK_STATUS_INITED, mac %s uuid %s \n", mac, uuid);
        if(STATION_GOT_IP == wifi_station_get_connect_status())
            wifi_led_set_status(WIFI_LED_CONNECTED_AP);
		break;
	case ALINK_STATUS_REGISTERED:
		sprintf(uuid, "%s", alink_get_uuid(NULL));
		wsf_deb("ALINK_STATUS_REGISTERED, mac %s uuid %s \n", mac, uuid);
		break;
	case ALINK_STATUS_LOGGED:
		sprintf(uuid, "%s", alink_get_uuid(NULL));
		wsf_deb("ALINK_STATUS_LOGGED, mac %s uuid %s\n", mac, uuid);
        wifi_led_set_status(WIFI_LED_CONNECTED_SERVER);
		break;
	case ALINK_STATUS_LOGOUT:
		sprintf(uuid, "%s", alink_get_uuid(NULL));
		wsf_deb("ALINK_STATUS_LOGOUT, mac %s uuid %s\n", mac, uuid);
		break;        
	default:
		break;
	}
	return 0;
}

/* fill device info �����豸��Ϣ��Ҫ�޸Ķ�Ӧ�궨��,����DEV_MAC��DEV_CHIPID ��Ҫ�û��Լ�ʵ�ֽӿں���*/
void ICACHE_FLASH_ATTR alink_fill_deviceinfo(struct device_info *deviceinfo)
{
	uint8 macaddr[6];
	//fill main device info here
	strcpy(deviceinfo->name, DeviceCustomInfo->name);
	strcpy(deviceinfo->sn, DeviceCustomInfo->sn);
	strcpy(deviceinfo->brand, DeviceCustomInfo->brand);
	strcpy(deviceinfo->key, DeviceCustomInfo->key);
	strcpy(deviceinfo->model, DeviceCustomInfo->model);
	strcpy(deviceinfo->secret, DeviceCustomInfo->secret);
	strcpy(deviceinfo->type, DeviceCustomInfo->type);
	strcpy(deviceinfo->version, DeviceCustomInfo->version);
	strcpy(deviceinfo->category, DeviceCustomInfo->category);
	strcpy(deviceinfo->manufacturer, DeviceCustomInfo->manufacturer);
	strcpy(deviceinfo->key_sandbox, DeviceCustomInfo->key_sandbox);
	strcpy(deviceinfo->secret_sandbox, DeviceCustomInfo->secret_sandbox);

	if (wifi_get_macaddr(0, macaddr)) {
		wsf_deb("macaddr=%02x:%02x:%02x:%02x:%02x:%02x\n", MAC2STR(macaddr));
		snprintf(deviceinfo->mac, sizeof(deviceinfo->mac), "%02x:%02x:%02x:%02x:%02x:%02x", MAC2STR(macaddr));
	}

	snprintf(deviceinfo->cid, sizeof(deviceinfo->cid), "%024d", system_get_chip_id());
	wsf_deb("DEV_MODEL:%s \n", deviceinfo->model);
}

static int ICACHE_FLASH_ATTR write_config(unsigned char *buffer, unsigned int len)
{
	int res = 0, pos = 0;

	if (buffer == NULL) {
		return ALINK_ERR;
	}
	if (len > ALINK_CONFIG_LEN)
		len = ALINK_CONFIG_LEN;

	res = spi_flash_erase_sector(ALINK_CONFIG_START_ADDR / 4096);	//one sector is 4KB 
	if (res != SPI_FLASH_RESULT_OK) {
		wsf_err("erase flash data fail\n");
	} else {
		wsf_err("erase flash data %d Byte\n", res);
	}
	os_printf("write data:%s\n",buffer);


	res = spi_flash_write((ALINK_CONFIG_START_ADDR), (uint32 *) buffer, len);
	if (res != SPI_FLASH_RESULT_OK) {
		wsf_err("write data error\n");
		return ALINK_ERR;
	}
	wsf_deb("write key(%s) success.\n", buffer);
	return ALINK_OK;
}

static int ICACHE_FLASH_ATTR read_config(unsigned char *buffer, unsigned int len)
{

	int res = 0;
	int pos = 0;
	res = spi_flash_read(ALINK_CONFIG_START_ADDR, (uint32 *) buffer, len);
	if (res != SPI_FLASH_RESULT_OK) {
		wsf_err("read flash data error\n");
		return ALINK_ERR;
	}

	os_printf("read data ok\n");
    
	return ALINK_OK;
}

int ICACHE_FLASH_ATTR alink_get_debuginfo(info_type type, char *status)
{
	int used;  
	switch (type) {    
		case MEMUSED:    
			used = 100 - ((system_get_free_heap_size()*100)/(96*1024));   
			sprintf(status, "%d%%", used);    
			break;    
		case WIFISTRENGTH:    
			sprintf(status , "%ddB",wifi_station_get_rssi());    
			break;    
		default:    
			status[0] = '\0';    
			break;  
	}  
	return 0;
}
int esp_ota_firmware_update( char * buffer, int len)
{
    os_printf("esp_ota_firmware_update \n");
   return upgrade_download(buffer , len);
}

int esp_ota_upgrade(void)
{
    os_printf("esp_ota_upgrade \n");
    system_upgrade_reboot();
    return 0;
}
extern int need_notify_app;
extern int  need_factory_reset ;

void set_thread_stack_size(struct thread_stacksize * p_thread_stacksize)
{
    p_thread_stacksize->alink_main_thread_size = 0xc00;
    p_thread_stacksize->send_work_thread_size = 0x800;
    p_thread_stacksize->wsf_thread_size = 0x1000;
    p_thread_stacksize->func_thread_size = 0x800;
}
   
int ICACHE_FLASH_ATTR alink_demo()
{
	struct device_info main_dev;
	
	memset(&main_dev, 0, sizeof(main_dev));
	alink_fill_deviceinfo(&main_dev);	// �������PRD�������豸��Ϣ
	alink_set_loglevel(ALINK_LL_DEBUG | ALINK_LL_INFO | ALINK_LL_WARN | ALINK_LL_ERROR);
	main_dev.sys_callback[ALINK_FUNC_SERVER_STATUS] = alink_handler_systemstates_callback;
	alink_set_callback(ALINK_FUNC_AVAILABLE_MEMORY, print_mem_callback);

	alink_register_cb(ALINK_FUNC_READ_CONFIG, (void *)&read_config);
	alink_register_cb(ALINK_FUNC_WRITE_CONFIG, (void *)&write_config);
	alink_register_cb(ALINK_FUNC_GET_STATUS, alink_get_debuginfo);
    alink_register_cb(ALINK_FUNC_OTA_FIRMWARE_SAVE, esp_ota_firmware_update);
    alink_register_cb(ALINK_FUNC_OTA_UPGRADE, esp_ota_upgrade);
	/*start alink-sdk */
    set_thread_stack_size(&g_thread_stacksize);
#ifdef PASS_THROUGH		
	alink_start_rawdata(&main_dev, rawdata_get_callback, rawdata_set_callback);
#else // ��͸����ʽ(�豸�����������json��ʽ����ͨѶ)
	main_dev.dev_callback[ACB_GET_DEVICE_STATUS] = main_dev_get_device_status_callback;
	main_dev.dev_callback[ACB_SET_DEVICE_STATUS] = main_dev_set_device_status_callback;
	
	alink_start(&main_dev);	//register main device here
#endif //PASS_THROUGH

	os_printf("%s %d wait time=%d \n", __FUNCTION__, __LINE__, ALINK_WAIT_FOREVER);

	ESP_DBG(("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"));
	if(ALINK_OK == alink_wait_connect(NULL, ALINK_WAIT_FOREVER))	//wait main device login, -1 means wait forever
	{
        os_printf("connect alink server ok\n");
	}
	else
	{
        os_printf("connect alink server failed\n");
	}  
    
    if(need_notify_app) {
        need_notify_app = 0;
        uint8 macaddr[6];
        char mac[17+1];
        if (wifi_get_macaddr(0, macaddr)) {
            os_printf("macaddr=%02x:%02x:%02x:%02x:%02x:%02x\n", MAC2STR(macaddr));
            snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x", MAC2STR(macaddr));
            zconfig_notify_app(DeviceCustomInfo->model, mac, ""); // if not factory reset , 
        }
    } 

	/* �豸�����ϱ����� */
	while (sample_running) {
        if (device_status_change) 
    		alink_device_post_data(NULL);
        
		alink_sleep(100);
	}

	/*  �豸�˳�alink-sdk */
	alink_end();

	return 0;
}

void ICACHE_FLASH_ATTR
hnt_param_array_regist(deviceParameter_t *custom_param_array, uint32 array_size)
{
    DeviceCustomParamTable.tableSize = array_size;
    DeviceCustomParamTable.table = custom_param_array;
}

void ICACHE_FLASH_ATTR
hnt_custom_info_regist(customInfo_t *customInfo)
{
    DeviceCustomInfo = customInfo;   
}

void ICACHE_FLASH_ATTR
hnt_device_status_change(void)
{
    device_status_change = 1;   
}

char* vendor_get_model(void)
{
    return DeviceCustomInfo->model;
}

