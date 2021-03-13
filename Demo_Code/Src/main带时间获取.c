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

// �¼�����
#define IDEL_EVEN (1L << 0) // �����¼�
#define KEY0_EVEN (1L << 1) // ����0�¼�
#define WKUP_EVEN (1L << 2) // ����wakeup�¼�
#define IR_EVEN (1L << 3) // ���⴫���������¼�

#define MSGSIZE 48 // GET���󷵻ص���Ϣ����
#define UrlLength 106 // ���ӵ�url����

// esp8266��غ궨��
#define User_ESP8266_ApSsid "veis"     //Ҫ���ӵ��ȵ������
#define User_ESP8266_ApPwd "123456789" //Ҫ���ӵ��ȵ����Կ

#define User_ESP8266_TcpServer_IP "192.168.137.22" //Ҫ���ӵ�TCP��������IP
#define User_ESP8266_TcpServer_Port "8080"          //Ҫ���ӵ�TCP�������Ķ˿�

#define TimeServerIP "api.k780.com"
#define HttpPort "80"
// ��ض��Ĳ����������������˽�API��ַ https://www.nowapi.com/api
#define AppKey "46110"
#define Sign "93ee0233e57e3afa6507ff2cbb9909a3"

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t even_flag = 0;              // �¼���ʶ��������ʾ32���¼�
extern __IO uint8_t ucTcpClosedFlag; // ��־TCP����������״̬
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Systick_Config(void);
void Esp8266_ConnectServer(ENUM_NetPro_TypeDef netpro, const char *ip_addr, const char *port);
void Esp8266_ConnectAp(const char *ssid, const char *password);
const char *CreateGetUrl(void); // ����GET�����ַ���

// �Լ�ʵ�ֵ��ַ�����������Ч������ڱ�׼��Ҫ��
const char *MyStrstr(const char *src, const char *sub); // �ַ������Һ���
const char *MyStrcat(char *aim, const char *insert);    // �ַ���ƴ�Ӻ���
int MyStrlen(char *aim);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

char linkstr[UrlLength] = {0}; // ���ڴ洢Get�����ַ���
DHT11_Data_TypeDef DHT11_Data; // �洢DHT11���ص�����

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t ucStatus;
  uint8_t count = 0;
  char time_message[MSGSIZE] = {0};

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  Systick_Config();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  DHT11_Init();

  ESP8266_Init();
//  char ssid[20];
//  char password[20];
//  printf("������WIFI���ƣ�");
//  scanf("%s",ssid);
//  setbuf(stdin,NULL);
//  printf("%s\r\n",ssid);
//  printf("������WIFI���룺");
//  scanf("%s",password);
//  setbuf(stdin,NULL);
//  printf("%s\r\n",password);
//  Esp8266_ConnectAp(ssid, password);                // ����·������ʵ������
  Esp8266_ConnectAp(User_ESP8266_ApSsid, User_ESP8266_ApPwd);                // ����·������ʵ������
//  Esp8266_ConnectServer(enumTCP, TimeServerIP, HttpPort);     // ������ʱ������
  __HAL_UART_ENABLE_IT(&husartx_esp8266, UART_IT_IDLE);       // ʹ��֡�ж�
//  // ��ȡʱ���GET����
//  char *url = (char *)CreateGetUrl();
//  ESP8266_Usart("%s", url);
//  //  ESP8266_Usart("GET http://api.k780.com/?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json\r\n");

//  while (1)
//  {
//    if (strEsp8266_Fram_Record.InfBit.FramFinishFlag)
//    {
//      strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength] = '\0';
//      //      printf ( "\r\n%s\r\n", strEsp8266_Fram_Record .Data_RX_BUF );
//      sprintf(time_message, "%s", strEsp8266_Fram_Record.Data_RX_BUF); // ��ʱ���������ݿ�����ʱ����Ϣ������
//      ESP8266_Clear_Buffer();
//      ESP8266_ExitUnvarnishSend();
//      count++;
//      if (count > 1)
//      {
//        ESP8266_Usart("AT+CIPCLOSE\r\n");
//        HAL_Delay(1000);
//        //          printf("%s\r\n",time_message);
//        HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); // ָʾ�Ƶ���
//        break;
//      }
//    }
//  }

//  // ����ʱ������ڵ��ַ���
//  printf("\r\n");
//  char *time1 = (char *)MyStrstr(time_message, "\"datetime_1\":\""); // ����������ƥ��
//  time1 = time1 + MyStrlen("\"datetime_1\":\""); // �����ַƫ��
//  while (*time1 != '"')
//    printf("%c", *time1++);
//  printf("\r\n");
 
  ESP8266_Link_Server(enumTCP, User_ESP8266_TcpServer_IP, User_ESP8266_TcpServer_Port, Single_ID_0);
  // ��һ�����ݶ���
  DHT11_Read_TempAndHumidity(&DHT11_Data);
  HAL_Delay(1200);
  // OLED����ʾ�ʺ���HELLO OLED
  OLED_ShowString(30, 0, (unsigned char *)"HELLO OLED", 16);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    
    /* USER CODE BEGIN 3 */
    
    if (DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS)
    {
  //    printf("��ȡDHT11�ɹ�!-->ʪ��Ϊ%.0f��RH ���¶�Ϊ %.0f�� \r\n", DHT11_Data.humidity, DHT11_Data.temperature);
  //    ESP8266_Usart("��ȡDHT11�ɹ�!-->ʪ��Ϊ%.0f��RH ���¶�Ϊ %.0f�� \r\n", DHT11_Data.humidity, DHT11_Data.temperature);
      // ��ʾʪ��
      OLED_ShowNum(0, 2, DHT11_Data.humidity, 2, 16);
      OLED_ShowString(17, 2, (unsigned char *)" %RH", 16);
      // ��ʾ�¶�
      OLED_ShowNum(0, 4, DHT11_Data.temp_high8bit, 2, 16);
      OLED_ShowString(18, 4, (unsigned char *)" 'C", 16);
    }
    else
    {
      printf("��ȡDHT11��Ϣʧ��\n");
    }

    if (strEsp8266_Fram_Record.InfBit.FramFinishFlag)
    {
      strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength] = '\0';
      if(esp8266_rxdata == '1')
      {
          HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET);
      }
      if(esp8266_rxdata == '0')
      {
          HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_RESET);
      }
      if(esp8266_rxdata == '2')
      {
          char str1[50] = {0};
          sprintf(str1,"temperature = %.0f C\n",DHT11_Data.temperature);
          ESP8266_SendString(DISABLE,str1,MyStrlen(str1),Single_ID_0);
      }
      if(esp8266_rxdata == '3')
      {
          char str2[50] = {0};
          sprintf(str2,"humidity = %.0f %%RH\n",DHT11_Data.humidity);
          ESP8266_SendString(DISABLE,str2,MyStrlen(str2),Single_ID_0);
      }
  //    printf("\r\n%s\r\n", strEsp8266_Fram_Record.Data_RX_BUF);
  //    ESP8266_Usart("��ȡDHT11�ɹ�!-->ʪ��Ϊ%.0f��RH ���¶�Ϊ %.0f�� \r\n", DHT11_Data.humidity, DHT11_Data.temperature);
      ESP8266_Clear_Buffer();
    }
    if (ucTcpClosedFlag) // ����Ƿ�ʧȥ����
    {
      ESP8266_ExitUnvarnishSend(); // �˳�͸��ģʽ
      do
        ucStatus = ESP8266_Get_LinkStatus(); // ��ȡ����״̬
      while (!ucStatus);
      if (ucStatus == 4) // ȷ��ʧȥ���Ӻ�����
      {
        printf("���������ȵ�ͷ����� ......\n");
        while (!ESP8266_JoinAP(User_ESP8266_ApSsid, User_ESP8266_ApPwd));
        while (!ESP8266_Link_Server(enumTCP, User_ESP8266_TcpServer_IP, User_ESP8266_TcpServer_Port, Single_ID_0));
        printf("�����ȵ�ͷ������ɹ�!!!\n");
      }
      while (!ESP8266_UnvarnishSend());
    }
    HAL_Delay(1200);
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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

// ���ӷ���������
void Esp8266_ConnectServer(ENUM_NetPro_TypeDef netpro, const char *ip_addr, const char *port)
{
  uint8_t count = 0;
  ESP8266_Enable_MultipleId(DISABLE);                                              // �رն�����
  while (!ESP8266_Link_Server(netpro, (char *)ip_addr, (char *)port, Single_ID_0)) // ���ӷ�����
  {
    ++count;
    if (5 == count)
    {
      printf("����������ʧ�ܣ�\r\n");
      return;
    }
  }
  while (!ESP8266_UnvarnishSend()); // ����͸��ģʽ
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

// ����GET������ַ���
const char *CreateGetUrl(void)
{
  const char *str1 = "GET http://api.k780.com";
  const char *str2 = "/?app=life.time&appkey=";
  const char *str3 = "&sign=";
  const char *str4 = "&format=json\r\n";
  MyStrcat(linkstr, str1);
  MyStrcat(linkstr, str2);
  MyStrcat(linkstr, AppKey);
  MyStrcat(linkstr, str3);
  MyStrcat(linkstr, Sign);
  MyStrcat(linkstr, str4);
  linkstr[UrlLength - 1] = '\0';
  return linkstr;
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

#ifdef  USE_FULL_ASSERT
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
