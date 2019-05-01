

#include"INA219.h"
#include"I2CIntr.h"



static uint8_t Intr_TxRx[20];

void I2CIntr_Test()
{    
    static I2CIntr I2CDevIntr(Gpio::B6, Gpio::B7,100000U);
    static INA219 INA219_Dev(&I2CDevIntr,0x80U);
    static float Voltage,Current;
    int16_t curr;
    uint8_t testID = 0;  
    uint8_t reg;
    uint16_t value;
    uint16_t slaveaddress = 0x08<<1;
    uint8_t name[] = "Amit Chaudhary is a good Boy";
    name[sizeof(name)/sizeof(uint8_t)-1] = '\n';
    //I2CDevIntr.HwInit();
    //I2CDevIntr.StartListening();
    //I2CDevIntr.InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_ENABLE_ALL);
    INA219_Dev.HwInit();
   // INA219_Dev.SetCalibration_16V_400mA();
    testID = 6;
    
    while(1)
    {
        switch(testID)
        {
        case 0:  // Receive 16 bytes
            I2CDevIntr.MasterTx(slaveaddress,name,sizeof(name)/sizeof(uint8_t)); 
            break;             
        case 1:  // Receive 6 bytes
            I2CDevIntr.MasterRx(slaveaddress,Intr_TxRx,6); 
            testID = 6;
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            break;                
        case 2:  // Receive 3 bytes
            I2CDevIntr.MasterRx(slaveaddress,Intr_TxRx,3); 
            break;            
        case 3:  // Receive 2 bytes
            I2CDevIntr.MasterRx(slaveaddress,Intr_TxRx,2); 
            break;
        case 4:  // Receive 1 bytes
            I2CDevIntr.MasterRx(slaveaddress,Intr_TxRx,1); 
            break;
        case 5:  // Ina219
            reg = 5;
            value = 0x2800;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            I2CDevIntr.MasterTx(0x80,Intr_TxRx,3); 
            break;
        case 6:              
            // Set Calibration register to 'Cal' calculated above
            // Write three bytes
            reg = 5;
            value = 4096;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            I2CDevIntr.MasterTx(0x80,Intr_TxRx,3);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            // Set Config register to take into account the settings above
            uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                                INA219_CONFIG_GAIN_8_320MV |
                                INA219_CONFIG_BADCRES_12BIT |
                                INA219_CONFIG_SADCRES_12BIT_128S_69MS | // INA219_CONFIG_SADCRES_12BIT_128S_69MS  INA219_CONFIG_SADCRES_12BIT_1S_532US
                                INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
            reg = 0;
            value = config;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            I2CDevIntr.MasterTx(0x80,Intr_TxRx,3);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            testID = 7;
            break;
        case 7:    
            /* Get Voltage */ 
            reg = 2;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTxRx(0x80,&Intr_TxRx[0],1,&Intr_TxRx[5],2,true);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            curr = ((Intr_TxRx[5] << 8) | Intr_TxRx[6]);
            Voltage = (int16_t)((curr >> 3) * 4);
            Voltage = Voltage * 0.001;
            
            /* Calibration value*/
            reg = 5;
            value = 4096;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            I2CDevIntr.MasterTx(0x80,Intr_TxRx,3);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            /* Get Current */ 
            reg = 4;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTxRx(0x80,&Intr_TxRx[0],1,&Intr_TxRx[5],2,true);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            curr = ((Intr_TxRx[5] << 8) | Intr_TxRx[6]);
            Current = curr/10;  
            testID = 8;
            break;            
        case 8:  // Repeated Start
            Intr_TxRx[0] = 'A';
            Intr_TxRx[1] = 'M';
            Intr_TxRx[2] = 'I';
            Intr_TxRx[3] = 'T';
            I2CDevIntr.MasterTxRx(slaveaddress,&Intr_TxRx[0],4,&Intr_TxRx[5],6,true); 
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            testID = 7;
            break;   
        default: break;
        
        }
        
        LL_mDelay(2);
    }
}