

#include"INA219.h"
#include"I2CIntr.h"
#define Q_RESPIRE_DELAY 200 

static I2CIntr I2CDevIntr(Gpio::B6, Gpio::B7,100000U);
static uint8_t Intr_TxRx[20];
static GpioOutput B13(Gpio::B13);
static HAL::I2CIntr::Transaction_t Transaction,Transaction1,Transaction2;
static uint8_t VoltageReg = 2,CurrentReg = 4;
static uint16_t VoltageValue,CurrentValue;
static I2CIntr::I2CStatus_t Status,CurrentTxnStatus,VoltageTxnStatus;
class I2CCallback : public Callback
{
    void CallbackFunction()
    {
        B13.Toggle();
    }
};

class I2CRxFullCallback_t : public Callback
{
    void CallbackFunction()
    {
        uint8_t data = 0;

        while(I2CDevIntr.SlaveRxDataAvailable())
        {
            data = I2CDevIntr.ReadRxData();
            data++;
        }
    }
};

class VoltageCallback_t : public Callback
{
    void CallbackFunction()
    {
        static float CallbackVoltage;
        int16_t curr;
        
        curr = ((VoltageValue<<8) & 0xff00) | ((VoltageValue>>8) & 0x00ff);
        CallbackVoltage = (int16_t)((curr >> 3) * 4);        
        CallbackVoltage = CallbackVoltage * 0.001;  
        //TxnDone = 1;
    }
};

class CurrentCallback_t : public Callback
{
    void CallbackFunction()
    {
        static float CallbackCurrent;
        int16_t curr;
        
        curr = ((CurrentValue<<8) & 0xff00) | ((CurrentValue>>8) & 0x00ff);
        CallbackCurrent = curr/10.0;  
        //TxnDone = 1;
    }
};

static class VoltageCallback_t  VoltageCallback;
static class CurrentCallback_t  CurrentCallback;

class I2CRxDoneCallback_t : public Callback
{
    void CallbackFunction()
    {
        while(I2CDevIntr.SlaveRxDataAvailable() > 1)
        {
            I2CDevIntr._I2CSlaveTxQueue.Write(I2CDevIntr.ReadRxData());
        }
        I2CDevIntr._I2CSlaveTxQueue.Write(I2CDevIntr.ReadRxData()+1);        
    }
};

void I2CIntr_Test()
{        
    static INA219 INA219_Dev(&I2CDevIntr,0x80U);
    static float Voltage,Current;
    
    static class I2CCallback I2C_XferDone_Callback;
    static class I2CRxFullCallback_t I2CRxQueueFullCallback;
    static class I2CRxDoneCallback_t I2CRxDoneCallback;
    uint8_t RepeatedStart;
   
    static uint32_t Q_Sucess,Q_Fail;
    int16_t curr;
    uint16_t config;
    uint8_t testID = 0;  
    uint8_t reg;
    uint16_t value;
    uint16_t slaveaddress = 0x08<<1;
    uint8_t name[] = "Amit Chaudhary is a good Boy";
    name[sizeof(name)/sizeof(uint8_t)-1] = '\n';
    INA219_Dev.HwInit();
    
    B13.HwInit();
    Transaction.XferDoneCallback = &I2C_XferDone_Callback;
    
    I2CDevIntr.SetCallback(HAL::I2CIntr::I2C_RX_QUEUE_FULL_CALLBACK,&I2CRxQueueFullCallback);
    I2CDevIntr.SetCallback(HAL::I2CIntr::I2C_SLAVE_RX_COMPLETE_CALLBACK,&I2CRxDoneCallback);
    //I2CDevIntr.StartListening();
    
    testID = 0;
    
    while(1)
    {
        switch(testID)
        {
        case 0:  // Send 16 bytes
            I2CDevIntr.MasterTx(slaveaddress,name,sizeof(name)/sizeof(uint8_t),&Status); 
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            testID = 16;
            break;             
        case 1:  // Receive 6 bytes
            I2CDevIntr.MasterRx(slaveaddress,Intr_TxRx,6,&Status); 
            testID = 6;
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            break;                
        case 2:  // Receive 3 bytes
            I2CDevIntr.MasterRx(slaveaddress,Intr_TxRx,3,&Status); 
            break;            
        case 3:  // Receive 2 bytes
            I2CDevIntr.MasterRx(slaveaddress,Intr_TxRx,2,&Status); 
            break;
        case 4:  // Receive 1 bytes
            I2CDevIntr.MasterRx(slaveaddress,Intr_TxRx,1,&Status); 
            break;
        case 5:  // Ina219
            reg = 5;
            value = 0x2800;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            I2CDevIntr.MasterTx(0x80,Intr_TxRx,3,&Status); 
            break;
        case 6:              
            // Set Calibration register to 'Cal' calculated above
            // Write three bytes
            reg = 5;
            value = 4096;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            I2CDevIntr.MasterTx(0x80,Intr_TxRx,3,&Status);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            // Set Config register to take into account the settings above
            // Write three bytes
            config = INA219_CONFIG_BVOLTAGERANGE_32V |
                        INA219_CONFIG_GAIN_8_320MV |
                        INA219_CONFIG_BADCRES_12BIT |
                        INA219_CONFIG_SADCRES_12BIT_128S_69MS |
                        INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
            reg = 0;
            value = config;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            I2CDevIntr.MasterTx(0x80,Intr_TxRx,3,&Status);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            testID = 13;
            break;
            
        case 7:    
            // INA219 Test with repeated start 
            // Get Voltage
            reg = 2;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTxRx(0x80,&Intr_TxRx[0],1,&Intr_TxRx[5],2,true,&Status);
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
            I2CDevIntr.MasterTx(0x80,Intr_TxRx,3,&Status);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            /* Get Current */ 
            reg = 4;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTxRx(0x80,&Intr_TxRx[0],1,&Intr_TxRx[5],2,1,&Status);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            curr = ((Intr_TxRx[5] << 8) | Intr_TxRx[6]);
            Current = curr;  
            Current = Current/10;
            testID = 15;
            break;
            
        case 8:    
            /* INA219 Test without repeated start*/
            /* Get Voltage */ 
            reg = 2;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTxRx(0x80,&Intr_TxRx[0],1,&Intr_TxRx[5],2,0,&Status);
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
            I2CDevIntr.MasterTx(0x80,Intr_TxRx,3,&Status);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            /* Get Current */ 
            reg = 4;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTxRx(0x80,&Intr_TxRx[0],1,&Intr_TxRx[5],2,0,&Status);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            curr = ((Intr_TxRx[5] << 8) | Intr_TxRx[6]);
            Current = curr/10;  
            testID = 13;
            break;
            
        case 9:  
            // TXRX with Repeated Start
            Intr_TxRx[0] = 'A';
            Intr_TxRx[1] = 'M';
            Intr_TxRx[2] = 'I';
            Intr_TxRx[3] = 'T';
            I2CDevIntr.MasterTxRx(slaveaddress,&Intr_TxRx[0],4,&Intr_TxRx[5],6,1,&Status); 
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            testID = 8;
            break; 
        case 10:  
            // TXRX without Repeated Start
            // Arduino sends NACK after sending some amount of data,
            // With repeated start(case 8) it works fine.
            Intr_TxRx[0] = 'A';
            Intr_TxRx[1] = 'M';
            Intr_TxRx[2] = 'I';
            Intr_TxRx[3] = 'T';
            I2CDevIntr.MasterTxRx(slaveaddress,&Intr_TxRx[0],4,&Intr_TxRx[5],2,0,&Status); 
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            break;  
            
        case 11:    
            /* INA219 Test without repeated start with "Transaction_t"*/
            /* Get Voltage */
            RepeatedStart = 0;
            reg = 2;
            Intr_TxRx[0] = reg;
            
            Transaction.SlaveAddress = 0x80;
            Transaction.TxBuf = &Intr_TxRx[0];
            Transaction.TxLen = 1;
            Transaction.RxBuf = &Intr_TxRx[5];
            Transaction.RxLen = 2;
            Transaction.RepeatedStart = RepeatedStart;
            Transaction.pStatus = &Status;
                
            I2CDevIntr.MasterTxRx(&Transaction);
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
            
            Transaction.SlaveAddress = 0x80;
            Transaction.TxBuf = &Intr_TxRx[0];
            Transaction.TxLen = 3;
            Transaction.RxBuf = nullptr;
            Transaction.RxLen = 0;
            Transaction.RepeatedStart = RepeatedStart;
            Transaction.pStatus = &Status;
                
            I2CDevIntr.MasterTx(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            /* Get Current */ 
            reg = 4;
            Intr_TxRx[0] = reg;            
            Transaction.SlaveAddress = 0x80;
            Transaction.TxBuf = &Intr_TxRx[0];
            Transaction.TxLen = 1;
            Transaction.RxBuf = &Intr_TxRx[5];
            Transaction.RxLen = 2;
            Transaction.RepeatedStart = RepeatedStart; 
            Transaction.pStatus = &Status;
                
            I2CDevIntr.MasterTxRx(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            curr = ((Intr_TxRx[5] << 8) | Intr_TxRx[6]);
            Current = curr/10;  
            break;
            
        case 12:  
            // TXRX with Repeated Start - transaction mode
            RepeatedStart = 1;
            
            Intr_TxRx[0] = 'A';
            Intr_TxRx[1] = 'M';
            Intr_TxRx[2] = 'I';
            Intr_TxRx[3] = 'T';
            
            Transaction.SlaveAddress = 0x10;
            Transaction.TxBuf = &Intr_TxRx[0];
            Transaction.TxLen = 4;
            Transaction.RxBuf = &Intr_TxRx[5];
            Transaction.RxLen = 0;
            Transaction.RepeatedStart = 0;
            Transaction.pStatus = &Status;
            
            I2CDevIntr.MasterTx(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            Transaction.SlaveAddress = 0x10;
            Transaction.TxBuf = &Intr_TxRx[0];
            Transaction.TxLen = 0;
            Transaction.RxBuf = &Intr_TxRx[5];
            Transaction.RxLen = 6;
            Transaction.RepeatedStart = 0;
            Transaction.pStatus = &Status;
            
            I2CDevIntr.MasterRx(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            Transaction.SlaveAddress = 0x10;
            Transaction.TxBuf = &Intr_TxRx[0];
            Transaction.TxLen = 4;
            Transaction.RxBuf = &Intr_TxRx[5];
            Transaction.RxLen = 6;
            Transaction.RepeatedStart = RepeatedStart;
            Transaction.pStatus = &Status;
            
            I2CDevIntr.MasterTxRx(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            break;            
        case 13:    
            /* INA219 Test without repeated start with "Transaction_t" and Queue Post*/
            /* Get Voltage */               
            RepeatedStart = 0;
            reg = 2;
            Intr_TxRx[0] = reg;            
            Transaction.SlaveAddress = 0x80;
            Transaction.TxBuf = &VoltageReg ;//&Intr_TxRx[0];
            Transaction.TxLen = 1;
            Transaction.RxBuf = (uint8_t*)&VoltageValue;
            Transaction.RxLen = 2;
            Transaction.RepeatedStart = RepeatedStart;
            Transaction.XferDoneCallback = &VoltageCallback;
            Transaction.pStatus = &VoltageTxnStatus;

            Status = I2CDevIntr.Post(&Transaction);
            
            if ((Status == I2CIntr::I2C_TXN_POSTED) || ( Status == I2CIntr::I2C_OK ) )
            {
                Q_Sucess++;
            }
            else 
            {
                Q_Fail++;
                LL_mDelay(Q_RESPIRE_DELAY);
            }
            
            /* Calibration value*/
            reg = 5;
            value = 4096;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            
            Transaction1.SlaveAddress = 0x80;
            Transaction1.TxBuf = &Intr_TxRx[0];
            Transaction1.TxLen = 3;
            Transaction1.RxBuf = nullptr;
            Transaction1.RxLen = 0;
            Transaction1.RepeatedStart = RepeatedStart;
            Transaction1.pStatus = &Status;
            
            Status = I2CDevIntr.Post(&Transaction1);
            if ((Status == I2CIntr::I2C_TXN_POSTED) || ( Status == I2CIntr::I2C_OK ) )
            {
                Q_Sucess++;
            }
            else 
            {
                Q_Fail++;
                LL_mDelay(Q_RESPIRE_DELAY);
            }
            
            /* Get Current */ 
            reg = 4;
            Intr_TxRx[0] = reg;            
            Transaction2.SlaveAddress = 0x80;
            Transaction2.TxBuf = &CurrentReg; //&Intr_TxRx[0];
            Transaction2.TxLen = 1;
            Transaction2.RxBuf = (uint8_t*)&CurrentValue; //&Intr_TxRx[5];
            Transaction2.RxLen = 2;
            Transaction2.RepeatedStart = RepeatedStart;
            Transaction2.XferDoneCallback = &CurrentCallback;
            Transaction2.pStatus = &CurrentTxnStatus;
            
            Status = I2CDevIntr.Post(&Transaction2);
            if ((Status == I2CIntr::I2C_TXN_POSTED) || ( Status == I2CIntr::I2C_OK ) )
            {
                Q_Sucess++;
            }
            else 
            {
                Q_Fail++;
                LL_mDelay(Q_RESPIRE_DELAY);
            }
            //testID = 7;
            break;    
        case 14:    
            /* INA219 Test with repeated start with "Transaction_t" and Queue Post*/
            /* Get Voltage */
            
            RepeatedStart = 1;
            reg = 2;
            Intr_TxRx[0] = reg;
            
            Transaction.SlaveAddress = 0x80;
            Transaction.TxBuf = &VoltageReg ;//&Intr_TxRx[0];
            Transaction.TxLen = 1;
            Transaction.RxBuf = (uint8_t*)&VoltageValue;
            Transaction.RxLen = 2;
            Transaction.RepeatedStart = RepeatedStart;
            Transaction.XferDoneCallback = &VoltageCallback;
            Transaction.pStatus = &VoltageTxnStatus;

            Status = I2CDevIntr.Post(&Transaction);
            
            if ((Status == I2CIntr::I2C_TXN_POSTED) || ( Status == I2CIntr::I2C_OK ) )
            {
                Q_Sucess++;
            }
            else 
            {
                Q_Fail++;
                LL_mDelay(Q_RESPIRE_DELAY);
            }
            
            /* Calibration value*/
            reg = 5;
            value = 4096;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            
            Transaction1.SlaveAddress = 0x80;
            Transaction1.TxBuf = &Intr_TxRx[0];
            Transaction1.TxLen = 3;
            Transaction1.RxBuf = nullptr;
            Transaction1.RxLen = 0;
            Transaction1.RepeatedStart = RepeatedStart;
            Transaction1.pStatus = &Status;
            
            Status = I2CDevIntr.Post(&Transaction1);
            if ((Status == I2CIntr::I2C_TXN_POSTED) || ( Status == I2CIntr::I2C_OK ) )
            {
                Q_Sucess++;
            }
            else 
            {
                Q_Fail++;
                LL_mDelay(Q_RESPIRE_DELAY);
            }
            
            /* Get Current */ 
            reg = 4;
            Intr_TxRx[0] = reg;            
            Transaction2.SlaveAddress = 0x80;
            Transaction2.TxBuf = &CurrentReg; //&Intr_TxRx[0];
            Transaction2.TxLen = 1;
            Transaction2.RxBuf = (uint8_t*)&CurrentValue; //&Intr_TxRx[5];
            Transaction2.RxLen = 2;
            Transaction2.RepeatedStart = RepeatedStart;
            Transaction2.XferDoneCallback = &CurrentCallback;
            Transaction2.pStatus = &CurrentTxnStatus;
            
            Status = I2CDevIntr.Post(&Transaction2);
            if ((Status == I2CIntr::I2C_TXN_POSTED) || ( Status == I2CIntr::I2C_OK ) )
            {
                Q_Sucess++;
            }
            else 
            {
                Q_Fail++;
                LL_mDelay(Q_RESPIRE_DELAY);
            }
            //testID = 7;
            break;    
            
            case 15: // DMA   
            /* INA219 Test without repeated start*/
                
            reg = 2;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTx_DMA(0x80,&Intr_TxRx[0],1,&Status);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            I2CDevIntr.MasterRx_DMA(0x80,&Intr_TxRx[5],2,&Status);
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
            I2CDevIntr.MasterTx_DMA(0x80,Intr_TxRx,3,&Status);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            /* Get Current */ 
            reg = 4;
            Intr_TxRx[0] = reg;           
            I2CDevIntr.MasterTx_DMA(0x80,&Intr_TxRx[0],1,&Status);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            I2CDevIntr.MasterRx_DMA(0x80,&Intr_TxRx[5],2,&Status);
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            curr = ((Intr_TxRx[5] << 8) | Intr_TxRx[6]);
            Current = curr;  
            Current = Current/10;
            LL_mDelay(100);
            //testID = 7;
            break;
            
        case 16:  // Send 16 bytes using DMA
            I2CDevIntr.MasterTx_DMA(slaveaddress,name,sizeof(name)/sizeof(uint8_t),&Status); 
            while(I2CDevIntr.GetState() != HAL::I2CIntr::READY);
            LL_mDelay(1000);
            break;  
            
            
        default: break;
        
        }
        
        //LL_mDelay(5);
    }
}