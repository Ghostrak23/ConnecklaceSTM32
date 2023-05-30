/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    App/custom_app.c
  * @author  MCD Application Team
  * @brief   Custom Example Application (Server)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "custom_app.h"
#include "custom_stm.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "logger.h"
#include "iks01a3_env_sensors.h"
#include "iks01a3_motion_sensors.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  /* First_service */
  uint8_t               Vit_Notification_Status;
  uint8_t               Temp_Notification_Status;
  uint8_t               Lat_Notification_Status;
  uint8_t               Long_Notification_Status;
  /* USER CODE BEGIN CUSTOM_APP_Context_t */
  // +BCH
  uint8_t 				swc_status;
  uint16_t 				cpt;
  /* USER CODE END CUSTOM_APP_Context_t */

  uint16_t              ConnectionHandle;
} Custom_App_Context_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern TIM_HandleTypeDef htim2;
/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/*typedef struct
{
	double latitude;
}CoordinatesLat;

typedef struct
{
	double longitude;
}CoordinatesLong;*/
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

static Custom_App_Context_t Custom_App_Context;

/**
 * END of Section BLE_APP_CONTEXT
 */

uint8_t UpdateCharData[247];
uint8_t NotifyCharData[247];

/* USER CODE BEGIN PV */
typedef struct
{
    char latitude[10];
} CoordinatesLat;

typedef struct
{
    char longitude[10];
} CoordinatesLong;

CoordinatesLat coordinatesLat[] = {
    {"43.152622"},
    {"43.152952"},
    {"43.153330"},
    {"43.153703"},
    {"43.154258"},
    {"43.154608"},
    {"43.154877"}
};

CoordinatesLong coordinatesLong[] = {
    {"6.018283"},
    {"6.018632"},
    {"6.019191"},
    {"6.019560"},
    {"6.019744"},
    {"6.019394"},
    {"6.018965"}
};

typedef struct DateAxes {
	float vitesse;
	IKS01A3_MOTION_SENSOR_Axes_t axes;
}date_axes;

typedef struct DateEnv {
	float temp;
}date_env;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* First_service */
static void Custom_Vit_Update_Char(void);
static void Custom_Vit_Send_Notification(void);
static void Custom_Temp_Update_Char(void);
static void Custom_Temp_Send_Notification(void);
static void Custom_Lat_Update_Char(void);
static void Custom_Lat_Send_Notification(void);
static void Custom_Long_Update_Char(void);
static void Custom_Long_Send_Notification(void);

/* USER CODE BEGIN PFP */
// +BCH
static void Custom_Ledc_Update_Char();
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void Custom_STM_App_Notification(Custom_STM_App_Notification_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_1 */
	// copy data update buffer +BCH
	memcpy(UpdateCharData,pNotification->DataTransfered.pPayload,pNotification->DataTransfered.Length);
  /* USER CODE END CUSTOM_STM_App_Notification_1 */
  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* USER CODE END CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* First_service */
    case CUSTOM_STM_VIT_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_VIT_NOTIFY_ENABLED_EVT */
    	Custom_App_Context.Vit_Notification_Status = 1;
    	Custom_Vit_Send_Notification();
      /* USER CODE END CUSTOM_STM_VIT_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_VIT_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_VIT_NOTIFY_DISABLED_EVT */
    	Custom_App_Context.Vit_Notification_Status = 0;
      /* USER CODE END CUSTOM_STM_VIT_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT */
    	Custom_App_Context.Temp_Notification_Status = 1;
    	Custom_Temp_Send_Notification();
      /* USER CODE END CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT */
    	Custom_App_Context.Temp_Notification_Status = 0;
      /* USER CODE END CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_LAT_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_LAT_NOTIFY_ENABLED_EVT */
    	Custom_App_Context.Lat_Notification_Status = 1;
    	Custom_Lat_Send_Notification();
      /* USER CODE END CUSTOM_STM_LAT_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_LAT_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_LAT_NOTIFY_DISABLED_EVT */
    	Custom_App_Context.Lat_Notification_Status = 0;
      /* USER CODE END CUSTOM_STM_LAT_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_LONG_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_LONG_NOTIFY_ENABLED_EVT */
    	Custom_App_Context.Long_Notification_Status = 1;
    	Custom_Long_Send_Notification();
      /* USER CODE END CUSTOM_STM_LONG_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_LONG_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_LONG_NOTIFY_DISABLED_EVT */
    	Custom_App_Context.Long_Notification_Status = 0;
      /* USER CODE END CUSTOM_STM_LONG_NOTIFY_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_STM_App_Notification_default */

      /* USER CODE END CUSTOM_STM_App_Notification_default */
      break;
  }
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_2 */

  /* USER CODE END CUSTOM_STM_App_Notification_2 */
  return;
}

void Custom_APP_Notification(Custom_App_ConnHandle_Not_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_APP_Notification_1 */

  /* USER CODE END CUSTOM_APP_Notification_1 */

  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_APP_Notification_Custom_Evt_Opcode */

    /* USER CODE END P2PS_CUSTOM_Notification_Custom_Evt_Opcode */
    case CUSTOM_CONN_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_CONN_HANDLE_EVT */

      /* USER CODE END CUSTOM_CONN_HANDLE_EVT */
      break;

    case CUSTOM_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_DISCON_HANDLE_EVT */

      /* USER CODE END CUSTOM_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_APP_Notification_default */

      /* USER CODE END CUSTOM_APP_Notification_default */
      break;
  }

  /* USER CODE BEGIN CUSTOM_APP_Notification_2 */

  /* USER CODE END CUSTOM_APP_Notification_2 */

  return;
}

void Custom_APP_Init(void)
{
  /* USER CODE BEGIN CUSTOM_APP_Init */
	// +BCH
	HAL_TIM_Base_Start_IT(&htim2);
	IKS01A3_MOTION_SENSOR_Init(IKS01A3_LSM6DSO_0,MOTION_ACCELERO);
	IKS01A3_ENV_SENSOR_Init(IKS01A3_HTS221_0, ENV_TEMPERATURE);
	UTIL_SEQ_RegTask(1<<CFG_TASK_LAT_ID, UTIL_SEQ_RFU, Custom_Lat_Send_Notification);
	UTIL_SEQ_RegTask(1<<CFG_TASK_LONG_ID, UTIL_SEQ_RFU, Custom_Long_Send_Notification);
	UTIL_SEQ_RegTask(1<<CFG_TASK_VIT_ID, UTIL_SEQ_RFU, Custom_Vit_Send_Notification);
	UTIL_SEQ_RegTask(1<<CFG_TASK_TEMP_ID, UTIL_SEQ_RFU, Custom_Temp_Send_Notification);
	Custom_Lat_Update_Char();
	Custom_Long_Update_Char();
	Custom_Vit_Update_Char();
	Custom_Temp_Update_Char();
  /* USER CODE END CUSTOM_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
// +BCH
void FSVC_APP_B3_Button_Action(void)
{
  UTIL_SEQ_SetTask( 1<<CFG_TASK_B3_BUTTON_PUSHED_ID, CFG_SCH_PRIO_0);
  Custom_App_Context.swc_status = 3;

  return;
}

void GPS_LAT_Action(void)
{
  UTIL_SEQ_SetTask( 1<<CFG_TASK_LAT_ID, CFG_SCH_PRIO_0);
}

void GPS_LONG_Action(void)
{
  UTIL_SEQ_SetTask( 1<<CFG_TASK_LONG_ID, CFG_SCH_PRIO_0);
}

void Accelerometre_Action(void)
{
  UTIL_SEQ_SetTask( 1<<CFG_TASK_VIT_ID, CFG_SCH_PRIO_0);
}

void Temperature_Action(void)
{
  UTIL_SEQ_SetTask( 1<<CFG_TASK_TEMP_ID, CFG_SCH_PRIO_0);
}

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* First_service */
void Custom_Vit_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Vit_UC_1*/

  /* USER CODE END Vit_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_VIT, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Vit_UC_Last*/

  /* USER CODE END Vit_UC_Last*/
  return;
}

void Custom_Vit_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Vit_NS_1*/
	  	// +BCH

  	  	date_axes data= {.axes.x = 0, .axes.y = 0, .axes.z = 0};
	  	if (Custom_App_Context.Vit_Notification_Status == 1)
		{
		  updateflag = 1;

		  // Read the acceleration values from the IKS01A3 accelerometer:
		  IKS01A3_MOTION_SENSOR_GetAxes(IKS01A3_LSM6DSO_0, MOTION_ACCELERO, &data.axes);

		  data.vitesse = fabs(sqrt((data.axes.x * data.axes.x) + (data.axes.y * data.axes.y) + (data.axes.z * data.axes.z)) - 998);

		  // Convertir la valeur de vitesse en une chaîne de caractères
		  char vitesse_str[20]; // Déclarer une chaîne de caractères pour stocker la valeur de vitesse convertie
		  sprintf(vitesse_str, "%.2f", data.vitesse); // Convertir la valeur de vitesse en une chaîne de caractères avec deux décimales après la virgule

		  // Copier la chaîne de caractères convertie dans NotifyCharData
		  memcpy(&NotifyCharData[0], vitesse_str, sizeof(vitesse_str));

		  APP_DBG_MSG("-- CUSTOM APPLICATION SERVER  : INFORM CLIENT SPEED SENT \n");
		}
		else
		{
		  APP_DBG_MSG("-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n");
		}
  /* USER CODE END Vit_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_VIT, (uint8_t *)NotifyCharData);
  }

  /* USER CODE BEGIN Vit_NS_Last*/

  /* USER CODE END Vit_NS_Last*/

  return;
}

void Custom_Temp_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Temp_UC_1*/

  /* USER CODE END Temp_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_TEMP, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Temp_UC_Last*/

  /* USER CODE END Temp_UC_Last*/
  return;
}

void Custom_Temp_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Temp_NS_1*/
		  	// +BCH
	  	  	date_env data = {.temp = 0.0f};
		  	if (Custom_App_Context.Temp_Notification_Status == 1)
			{
			  updateflag = 1;

			  // Read the temperature value from the HTS221 sensor,
			  IKS01A3_ENV_SENSOR_GetValue(IKS01A3_HTS221_0, ENV_TEMPERATURE, &data.temp);

			  // Convertir la valeur de vitesse en une chaîne de caractères
			  char temperature_str[20]; // Déclarer une chaîne de caractères pour stocker la valeur de vitesse convertie
			  sprintf(temperature_str, "%.2f", data.temp);

			  // Copy a converted character string into NotifyCharData
			  memcpy(&NotifyCharData[0], temperature_str, sizeof(temperature_str));

			  APP_DBG_MSG("-- CUSTOM APPLICATION SERVER  : INFORM CLIENT TEMP SENT \n");
			}
			else
			{
			  APP_DBG_MSG("-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n");
			}
  /* USER CODE END Temp_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_TEMP, (uint8_t *)NotifyCharData);
  }

  /* USER CODE BEGIN Temp_NS_Last*/

  /* USER CODE END Temp_NS_Last*/

  return;
}

void Custom_Lat_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Lat_UC_1*/

  /* USER CODE END Lat_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_LAT, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Lat_UC_Last*/

  /* USER CODE END Lat_UC_Last*/
  return;
}

void Custom_Lat_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Lat_NS_1*/

    if (Custom_App_Context.Lat_Notification_Status == 1)
    {
        updateflag = 1;

        static int index = 0;
        if (index >= sizeof(coordinatesLat) / sizeof(coordinatesLat[0])) {
            index = 0;
        }

        // Copier la latitude dans NotifyCharData

        memcpy(&NotifyCharData[0], coordinatesLat[index].latitude, sizeof(coordinatesLat[index].latitude));

        index++;
        APP_DBG_MSG("-- CUSTOM APPLICATION SERVER  : INFORM CLIENT LATITUDE SENT \n");
    }
    else
    {
        APP_DBG_MSG("-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n");
    }

  /* USER CODE END Lat_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_LAT, (uint8_t *)NotifyCharData);
  }

  /* USER CODE BEGIN Lat_NS_Last*/

  /* USER CODE END Lat_NS_Last*/

  return;
}

void Custom_Long_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Long_UC_1*/

  /* USER CODE END Long_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_LONG, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Long_UC_Last*/

  /* USER CODE END Long_UC_Last*/
  return;
}

void Custom_Long_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Long_NS_1*/

		if (Custom_App_Context.Long_Notification_Status == 1)
	{
	updateflag = 1;

	static int index = 0;
	    if (index >= sizeof(coordinatesLong) / sizeof(coordinatesLong[0])) {
	        index = 0;
	    }

	    // Convertir la latitude en tableau de bytes

        memcpy(&NotifyCharData[0], coordinatesLong[index].longitude, sizeof(coordinatesLong[index].longitude));

	    // Appel de la fonction d'envoi avec l'UUID correspondant à la latitude

	    index++;

		  APP_DBG_MSG("-- CUSTOM APPLICATION SERVER  : INFORM CLIENT LATITUDE SENT \n");
	}
	else
	{
		APP_DBG_MSG("-- CUSTOM APPLICATION : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n");
	}

  /* USER CODE END Long_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_LONG, (uint8_t *)NotifyCharData);
  }

  /* USER CODE BEGIN Long_NS_Last*/

  /* USER CODE END Long_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
// +BCH
void Custom_Ledc_Update_Char() /* Property writed */
{
  return;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim2){
	  GPS_LAT_Action();
	  GPS_LONG_Action();
	  Accelerometre_Action();
	  Temperature_Action();
  }
}
/* USER CODE END FD_LOCAL_FUNCTIONS*/
