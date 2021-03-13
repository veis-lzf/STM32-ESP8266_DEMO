/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "dht11.h"
#include "esp8266.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

// esp8266��غ궨�壬�����Լ���AP��Server���޸����º궨��
#define User_ESP8266_ApSsid "veis"     // Ҫ���ӵ��ȵ������
#define User_ESP8266_ApPwd "123456789" // Ҫ���ӵ��ȵ����Կ

#define User_ESP8266_TcpServer_IP "192.168.137.22" // Ҫ���ӵ�TCP��������IP
#define User_ESP8266_TcpServer_Port "8080"         // Ҫ���ӵ�TCP�������Ķ˿�

#define TCP_Server_Port "8088"
#define TCP_Server_Timeout "1800"
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern __IO uint8_t ucTcpClosedFlag; // ��־TCP����������״̬
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Systick_Config(void);
void WIFI_Config(void);
void Esp8266_ConnectAp(const char *ssid, const char *password);
// ʵ�ֵ��ַ���������
const char *MyStrstr(const char *src, const char *sub); // �ַ������Һ���
const char *MyStrcat(char *aim, const char *insert);    // �ַ���ƴ�Ӻ���
int MyStrlen(char *aim);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
DHT11_Data_TypeDef DHT11_Data; // �洢DHT11���ص�����
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  Systick_Config();
  
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init(); // ��ʼ��OLED
  DHT11_Init(); // ��ʼ��DHT11
  ESP8266_Init(); // ��ʼ��ESP8266
  WIFI_Config();
  
  // ��һ�����ݶ���
  DHT11_Read_TempAndHumidity(&DHT11_Data);
  HAL_Delay(1200);
  // OLED����ʾ���� OLED Show
  OLED_Clear();
  OLED_ShowString(30, 0, (unsigned char *)"OLED Show", 16);
  /* USER CODE END 2 */

  while (1)
  {
    /* USER CODE BEGIN 3 */
    // ��ȡDHT11��ʪ�����ݲ����µ�OLED
    if (DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS)
    {
      char temp[20];
      char humid[20];
      sprintf(humid, "Humid: %0.f %%RH", DHT11_Data.humidity);
      sprintf(temp, "Temp:  %.0f C", DHT11_Data.temperature);
      OLED_ShowString(0, 2, (unsigned char *)temp, 16);
      OLED_ShowString(0, 4, (unsigned char *)humid, 16);
    }
    // ���µ�APP
    if (strEsp8266_Fram_Record.InfBit.FramFinishFlag)
    {
      strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength] = '\0';
      printf("���յ����ַ�Ϊ��%c\r\n", esp8266_rxdata);

      if (esp8266_rxdata == 'a') // ��LED0
      {
        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
        ESP8266_SendString(DISABLE, "LED ON", MyStrlen("LED ON"), Multiple_ID_0);
      }
      if (esp8266_rxdata == 'b') // �ر�LED0
      {
        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
        ESP8266_SendString(DISABLE, "LED OFF", MyStrlen("LED OFF"), Multiple_ID_0);
      }
      if (esp8266_rxdata == 'c') // ��ӡ�¶�����
      {
        char str1[50] = {0};
        sprintf(str1, "temperature:%.0f C", DHT11_Data.temperature);
        ESP8266_SendString(DISABLE, str1, MyStrlen(str1), Multiple_ID_0);
      }
      if (esp8266_rxdata == 'd') // ��ӡʪ������
      {
        char str2[50] = {0};
        sprintf(str2, "humidity:%.0f %%RH", DHT11_Data.humidity);
        ESP8266_SendString(DISABLE, str2, MyStrlen(str2), Multiple_ID_0);
      }
      ESP8266_Clear_Buffer();
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void WIFI_Config()
{
  char macip[20] = {0};
  char macip_s[30] = {0};
  char port[20] = {0};
  
  Esp8266_ConnectAp(User_ESP8266_ApSsid, User_ESP8266_ApPwd); // ����·������ʵ������
  printf("\r\n�����ȵ�\r\n");
  ESP8266_BuildAP("veis_esp8266","123456789",WPA_WPA2_PSK);
  ESP8266_ExitUnvarnishSend();
  ESP8266_Enable_MultipleId(ENABLE); 
  ESP8266_StartOrShutServer(ENABLE,TCP_Server_Port,TCP_Server_Timeout);
  ESP8266_Inquire_ApIp(macip, 20);

  printf("���� ESP8266 ���\n");
  __HAL_UART_ENABLE_IT(&husartx_esp8266, UART_IT_IDLE); // ʹ��֡�ж�
  
  sprintf(macip_s, "IP:%s",macip);
  sprintf(port, "Port:%s",TCP_Server_Port);
  // ��OLED����ʾ������Ϣ
  OLED_ShowString(0, 0, (unsigned char *)"ssid:veis_esp8266", 16);
  OLED_ShowString(0, 2, (unsigned char *)"key:123456789", 16);
  OLED_ShowString(0, 4, (unsigned char *)macip_s, 16);
  OLED_ShowString(0, 6, (unsigned char *)port, 16);

  while(1)
  {
      ESP8266_ReceiveString(DISABLE);
      if (strEsp8266_Fram_Record.InfBit.FramFinishFlag)
      {      
          if(esp8266_rxdata == 'e') // ����APP���˳�OLED��ʾ
          {
            break;
          }
      }
  }
  printf("\r\n����APP��ɣ�\r\n");

}

// ����AP����
void Esp8266_ConnectAp(const char *ssid, const char *password)
{
  uint8_t count = 0;
  printf("��ʼwifi����\r\n");
  ESP8266_Net_Mode_Choose(STA_AP); // ����ΪSTAģʽ
  ESP8266_Cmd("AT+RST", "OK", "ready", 2500);
  while (!ESP8266_JoinAP((char *)ssid, (char *)password)) // ����wifi
  {
    ++count;
    if (3 == count)
    {
      printf("wifi����ʧ�ܣ�\r\n");
      return;
    }
  }
  printf("����WIFI�ɹ���\r\nssid:%s\tpassword:%s\r\n", ssid, password);
}

// �ַ���ƴ�Ӻ���
const char *MyStrcat(char *a, const char *insert)
{
  char *address = a;
  while (*a != '\0')
    a++;
  while (*insert != '\0')
    *a++ = *insert++;
  *a = '\0';
  return address;
}

// �ַ������Һ���
// ���ҳɹ�����Ŀ���ַ����׵�ַ��
// ����ʧ�ܷ���0
const char *MyStrstr(const char *src, const char *sub)
{
  const char *bp, *sp;

  //	 �ж��Ƿ�Ϊ���ַ���
  if (!src || !sub)
  {
    return src;
  }
  //	 �����ַ���
  while (*src)
  {
    bp = src;
    sp = sub;
    do
    {
      if (!*sp) // ����sub�Ľ���λ�ã�����srcλ��
        return src;
    } while (*bp++ == *sp++);
    src++;
  }
  return NULL;
}

// �ַ���������ȡ����
int MyStrlen(char *aim)
{
  int count = 0;
  while (*aim++)
  {
    count++;
  }
  return count;
}

// ����Systick��ʱ����ز���
void Systick_Config(void)
{
  // HAL_RCC_GetHCLKFreq()/1000    1ms�ж�һ��
  // HAL_RCC_GetHCLKFreq()/100000	 10us�ж�һ��
  // HAL_RCC_GetHCLKFreq()/1000000 1us�ж�һ��
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000); // ���ò�����ϵͳ�δ�ʱ��
  // ϵͳ�δ�ʱ��ʱ��Դ
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  // ϵͳ�δ�ʱ���ж����ȼ�����
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

// ���ڽ�����ɻص�����
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  if (strEsp8266_Fram_Record.InfBit.FramLength < (RX_BUF_MAX_LEN - 1)) // Ԥ��1���ֽ�д������
    strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength++] = esp8266_rxdata;
  HAL_UART_Receive_IT(&husartx_esp8266, &esp8266_rxdata, 1);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
