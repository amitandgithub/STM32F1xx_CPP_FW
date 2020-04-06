


// #include"Ir_Test.h"

#include "string.h"
#include "IRremote.h"
#include"CPP_HAL.h"

int Ir_Test(void);

TIM_HandleTypeDef htim4;


static void MX_TIM4_Init(void);
static void MX_GPIO_Init(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);

extern decode_results results;
int Ir_Test(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  //HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  //SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

  char trans_str[96] = {0,};
  //snprintf(trans_str, 64, "IR-rec-trans https://istarik.ru/blog/stm32/125.html\n");
  //HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 100);

  my_enableIRIn(); // ?????????????

  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  static const char *decode_str[] = {"UNUSED", "UNKNOWN", "RC5", "RC6", "NEC", "SONY", "PANASONIC", "JVC", "SAMSUNG", "WHYNTER", "AIWA_RC_T501", "LG", "SANYO", "MITSUBISHI", "DISH", "SHARP", "DENON", "PRONTO"};

  while (1)
  {
	////////////////////////////////////////// ?????-????????????? ///////////////////////////////////////////////////
	if(my_decode(&results))
	{
		////////// ????? ????????????? ?????? ///////////////
		snprintf(trans_str, 96, "Cod: %p | Type: %s | Bits: %d\n", (void*)results.value, decode_str[results.decode_type + 1], results.bits);
		//HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 100);
                HAL::DBG_PRINT((uint8_t*)trans_str, strlen(trans_str));

		////////// ????? ?????? ? "?????" ???? ///////////////
		/*snprintf(trans_str, 96, "Raw: %d\n", results.rawlen);
		HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 100);

		for(int i = 1; i < results.rawlen; i++)
		{
			if(i & 1)
			{
				snprintf(trans_str, 96, "%d", results.rawbuf[i] * USECPERTICK);
				HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 100);
			}
			else
			{
				HAL_UART_Transmit(&huart1, (uint8_t*)"-", 1, 100);
				snprintf(trans_str, 96, "%lu", (uint32_t)results.rawbuf[i] * USECPERTICK);
				HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 100);
			}

			HAL_UART_Transmit(&huart1, (uint8_t*)" ", 1, 100);
		}

		HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n-----------------\r\n", strlen("\r\n-----------------\r\n"), 100);*/
		////////// ????? ?????? ?????? ? "?????" ???? ///////////////

		HAL_Delay(300); // ????????? ????? (???? ?? "????????"), ? ?? ????? ?????? ????????? ??????
		my_resume(); // ??????? ?????? (????? ???????????)
	}

	////////////////////////////////////////// ???????? ??????????????? ??????? ///////////////////////////////////////////////////
	// Cod: 0x707048b7 | Type: SAMSUNG | Bits: 32
	/*uint32_t cod_but = 0;
	cod_but = 0x707048b7; // ??? ?????? ????????? ??? ?????????????

	sendSAMSUNG(cod_but, 32); // ???????? ????????? ??? ?????????????, ??? ?????? ? ????? ?????? ? ????? (Cod: 0x707048b7 | Type: SAMSUNG | Bits: 32)
	my_enableIRIn(); // ????????????????????? ?????? (????? ???????????)
	HAL_Delay(2000);*/

	/////////////////////////////////// ???????? ?? ??????????????? ???????, ? ????? ???? /////////////////////////////////////////
	// Cod: 0xa32ab931 | Type: UNKNOWN | Bits: 32
	// Raw: 24
	// 1300 -400 1300 -400 450 -1200 1300 -400 1300 -400 450 -1200 500 -1200 450 -1250 450 -1200 500 -1200 450 -1250 1300

	/*uint8_t khz = 38; // ??????? ??????? ????????????????? ?????, ???????? ???????????? ?? 36 ?? 40 ???
	unsigned int raw_signal[] = {1300, 400, 1300, 400, 450, 1200, 1300, 400, 1300, 400, 450, 1200, 500, 1200, 450, 1250, 450, 1200, 500, 1200, 450, 1250, 1300};

	sendRaw(raw_signal, sizeof(raw_signal) / sizeof(raw_signal[0]), khz);
	my_enableIRIn(); // ????????????????????? ?????? (????? ???????????)
	HAL_Delay(2000);*/

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 0;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
   while(1);
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    while(1);
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    while(1);
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
   while(1);
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
   while(1);
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : recive_IR_Pin */
  GPIO_InitStruct.Pin = recive_IR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(recive_IR_GPIO_Port, &GPIO_InitStruct);

}


void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspPostInit 0 */

  /* USER CODE END TIM4_MspPostInit 0 */
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM4 GPIO Configuration    
    PB6     ------> TIM4_CH1 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM4_MspPostInit 1 */

  /* USER CODE END TIM4_MspPostInit 1 */
  }

}