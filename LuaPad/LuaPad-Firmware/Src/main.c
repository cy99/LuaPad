/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
	*.s�ļ��Ķ�ջ��С��Ҫ�����Ĵ�
	
	*PWMƵ��21kHz,���ý�0,3,4,5,6
	*LED2,LED3�ֱ�ΪPWM 5,6
	
	*ADC���ý�0,1,2,3,4,5,6,7
	*���ص�λ��Ϊ 8
	
	***Cube��������֮����Ҫ�޸ģ�
	1.sd_diskio.c��BSP_SD_ReadBlocks->BSP_SD_ReadBlocks_DMA
				   BSP_SD_WriteBlocks->BSP_SD_WriteBlocks_DMA
	2.���ʹ����micros()��������Ҫ�޸�
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000)Ϊ
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 100000)
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "mInclude.h"
#include "LuaPad.h"
#include "HAL_MPU6050.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
FATFS SDFatFs;              //FAT�ļ�ϵͳ����
FIL MyFile;                 //�ļ�����
extern char SD_Path[4];     //SD���߼�·��
FRESULT res;                //SD���������
uint32_t byteswritten, bytesread; //�ɹ�д��/��ȡ���ֽ���
char wtext[100];    //д������buf
char rtext[100];         //��ȡ����buf

MPU6050 mpu6050;

extern uint16_t ADCConvertedValue[9];  //ADC����ֵ
int i;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration----------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_ADC1_Init();
    MX_TIM3_Init();
    MX_TIM5_Init();
    MX_TIM12_Init();
    MX_SDIO_SD_Init();
    MX_FATFS_Init();
    MX_I2C1_Init();

    /* USER CODE BEGIN 2 */
    if(HAL_UART_Receive_IT(&huart1, uart1_rxBuffer, 1) != HAL_OK)
        Error_Handler();
    if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADCConvertedValue, 9) != HAL_OK)
        Error_Handler();
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);
    HAL_SD_Init(&hsd, &SDCardInfo);
    HAL_SD_WideBusOperation_Config(&hsd, SDIO_BUS_WIDE_4B);
    MPU6050_Init(&hi2c1, &mpu6050, MPU6050_Device_0, MPU6050_Accelerometer_2G,
                 MPU6050_Gyroscope_250s );

    HAL_Delay(50);


    if(f_mount(&SDFatFs, (TCHAR const*)SD_Path, 0) != FR_OK)
    {
        /* FatFs Initialization Error */
        Error_Handler();
    }
    else
    {
        if(f_open(&MyFile, "TM32.TXT", FA_READ) != FR_OK)
        {
            /* 'STM32.TXT' file Open for read Error */
            //Error_Handler();
        }
        else
        {
            res = f_read(&MyFile, rtext, sizeof(rtext), (UINT*)&bytesread);

            if((bytesread == 0) || (res != FR_OK))
            {
                /* 'STM32.TXT' file Read or EOF Error */
                Error_Handler();
            }
            else
            {
                f_close(&MyFile);

                if(rtext[0] == '1')
                    PAD_LED(1, ON);
                else if(rtext[0] == '0')
                    PAD_LED(1, OFF);
                else
                    PAD_LED(2, ON);
            }
        }
    }

    f_open(&MyFile, "IMU.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    f_close(&MyFile);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        int p = PAD_ADC(8) / 4;
        PAD_PWM(0, p);
        PAD_PWM(LED3, 1024 - p);
        //printf("%d\n", PAD_ADC(0));
		
		f_open(&MyFile, "IMU.TXT", FA_WRITE);
		res = f_lseek(&MyFile, MyFile.fsize);
        while(PAD_Key() != GPIO_PIN_SET)
        {
            MPU6050_ReadTemperature(&hi2c1, &mpu6050);
            MPU6050_ReadGyroscope(&hi2c1, &mpu6050);
            MPU6050_ReadAccelerometer(&hi2c1, &mpu6050);

			printf("%.2f,%.2f,%.2f\r\n",mpu6050.Accelerometer_X*0.1f,mpu6050.Accelerometer_Y*0.1f,mpu6050.Accelerometer_Z*0.1f);
            int len = sprintf(wtext, "%d a/g: %d,%d,%d,%d,%d,%d\r\n", i,mpu6050.Accelerometer_X,
                        mpu6050.Accelerometer_Y, mpu6050.Accelerometer_Z, mpu6050.Gyroscope_X,
                        mpu6050.Gyroscope_Y, mpu6050.Gyroscope_Z);
            f_write(&MyFile, wtext, len, (void *)&byteswritten);
            
            PAD_LED(1, TURN);
        }
		f_close(&MyFile);
        FATFS_UnLinkDriver(SD_Path);

        while(1)
        {
            PAD_LED(1, TURN);
            HAL_Delay(300);
        }
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
//usart1�ص�����
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)
{
    if(UartHandle->Instance == USART1)
    {
        switch(uart1_rxBuffer[0])
        {
            case '1':
                PAD_LED(1, TURN);
                break;
            case '2':
                PAD_LED(2, TURN);
                break;
            case '3':
                PAD_LED(3, TURN);
                break;
        }
        HAL_UART_Receive_IT(&huart1, uart1_rxBuffer, 1);
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {
    }
    /* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    printf("Wrong parameters value: file %s on line %d\r\n", file, line)
    /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/