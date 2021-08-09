#include "main.h"
#include "stdio.h"
#include "fatfs.h"
#include "fatfs_sd.h"
#include "string.h"

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

FATFS fs;
FIL fil;
FRESULT fresult;
UINT bw,br;
char buffer[1024];

FATFS *pfs;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
void send_data_uart(char*);
int get_buffer_size(char*);
void clear_buffer(char*);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_FATFS_Init();

	  	  	  	  	  	  	  	  	  	 // SEARCH SD CARD //
  //-----------------------------------------------------------------------------------------------//

  strcpy(buffer,"SD KART BASLATILIYOR...\n");
  send_data_uart(buffer);
  clear_buffer(buffer);

  fresult = f_mount(&fs, "", 0);
  if (fresult != FR_OK) send_data_uart ("SD KART BULUNAMADI\n");
  else send_data_uart("SD KART BULUNDU...\n");

 	 	 	 	 	 	 	 	   	     // CARD CAPACITY //
  //-----------------------------------------------------------------------------------------------//

  fresult = f_getfree("", &fre_clust, &pfs);
  if(fresult != FR_OK) { send_data_uart ("SD KART BOS ALAN OKUMA BASARISIZ\n"); }
  else if(fresult == FR_OK)
  {
	  send_data_uart ("SD KART BOS ALAN OKUMA BASARILI\n");
	  totalSpace = (uint32_t)((pfs->n_fatent -2) * pfs->csize * 0.5);
	  freeSpace = (uint32_t)(fre_clust * pfs->csize * 0.5);

	  send_data_uart("SD CARD TOPLAM ALAN: ");
	  sprintf(buffer,"%lu",totalSpace);
	  send_data_uart(buffer);
	  send_data_uart("\n");
	  clear_buffer(buffer);

	  send_data_uart("SD CARD BOS ALAN: ");
	  sprintf(buffer,"%lu",freeSpace);
	  send_data_uart(buffer);
	  send_data_uart("\n");
	  clear_buffer(buffer);
  }
  	  	  	  	  	  	  	  	 	 // FILE CREATING AND WRITING //
  //-----------------------------------------------------------------------------------------------//

  fresult = f_open(&fil, "TEST.txt", FA_CREATE_ALWAYS | FA_WRITE);
  if (fresult != FR_OK) send_data_uart ("DOSYA OLUSTURMA,ACMA VE YAZMA MODU AYARLAMA BASARISIZ\n");
  else send_data_uart("DOSYA OLUSTURMA,ACMA VE YAZMA MODU AYARLAMA BASARILI\n");

  strcpy (buffer, "FURKAN COSKUN");
  f_puts(buffer, &fil);
  send_data_uart ("YOLLANAN VERI: ");
  send_data_uart (buffer);
  send_data_uart ("\n");
  clear_buffer(buffer);

  fresult = f_close(&fil);
  if (fresult != FR_OK) send_data_uart ("DOSYA KAPAMA ISLEMI BASARISIZ\n");
  else send_data_uart("DOSYA KAPAMA ISLEMI BASARILI\n");

  	  	  	  	  	  	  	  	  	  // READING FROM FILE //
  //-----------------------------------------------------------------------------------------------//

  fresult = f_open(&fil, "TEST.txt", FA_READ);
  if (fresult != FR_OK) send_data_uart ("DOSYA ACMA ISLEMİ VE OKUMA MODU AYARLAMA BASARISIZ\n");
  else send_data_uart("DOSYA ACMA ISLEMI VE OKUMA MODU AYARLAMA BASARILI\n");

  f_read(&fil, buffer, fil.fsize, &br);
  send_data_uart ("OKUNAN VERI: ");
  send_data_uart (buffer);
  send_data_uart ("\n");
  clear_buffer(buffer);

  fresult = f_close(&fil);
  if (fresult != FR_OK) send_data_uart ("DOSYA KAPAMA ISLEMI BASARISIZ\n");
  else send_data_uart("DOSYA KAPAMA ISLEMI BASARILI\n");

  	  	  	  	  	  	  	  	  	  	// DELETION //
  //-----------------------------------------------------------------------------------------------//

  fresult = f_unlink("TEST.txt");
  send_data_uart ("DOSYA SILINIYOR\n");
  if (fresult != FR_OK) send_data_uart ("DOSYA SILME BASARISIZ\n");
  else send_data_uart("DOSYA SILME BASARILI\n");

  while (1) {}
}

void send_data_uart(char* string)
{
	uint8_t len = strlen(string);
	HAL_UART_Transmit(&huart2, (uint8_t*)string, len, 2000);
}

int get_buffer_size(char *buf)
{
	int i = 0;
	while(*buf++ != '\0') ++i;
	return i;
}

void clear_buffer(char * buffer)
{
	int len = strlen(buffer);
	for(int i = 0; i < len ; ++i)
	{
		buffer[i] = '\0';
	}
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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
  /** Initializes the CPU, AHB and APB buses clocks
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

static void MX_SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GREEN_LED_Pin|RED_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : GREEN_LED_Pin RED_LED_Pin */
  GPIO_InitStruct.Pin = GREEN_LED_Pin|RED_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
