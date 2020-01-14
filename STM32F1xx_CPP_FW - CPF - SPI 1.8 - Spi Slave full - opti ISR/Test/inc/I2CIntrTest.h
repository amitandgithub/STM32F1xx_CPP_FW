

#include"INA219.h"
#include"I2c.h"
#define Q_RESPIRE_DELAY 200 
uint32_t logidx = 0;
static I2c I2CDevIntr(Gpio::B6, Gpio::B7,400000U);
static uint8_t Intr_TxRx[20];
static GpioOutput B13(Gpio::B13);
static HAL::I2c::Transaction_t Transaction,Transaction1,Transaction2;
static uint8_t VoltageReg = 2,CurrentReg = 4;
static uint16_t VoltageValue,CurrentValue;
static I2c::I2CStatus_t Status,CurrentTxnStatus,VoltageTxnStatus;

#if (I2C_INT) || (I2C_DMA) 

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
            I2CDevIntr.m_I2CSlaveTxQueue.Write(I2CDevIntr.ReadRxData());
        }
        I2CDevIntr.m_I2CSlaveTxQueue.Write(I2CDevIntr.ReadRxData()+1);        
    }
};
#endif
void I2c_Test()
{        
    static INA219 INA219_Dev(&I2CDevIntr,0x80U);
    static float Voltage,Current;
#if (I2C_INT) || (I2C_DMA)   
    static class I2CCallback I2C_XferDone_Callback;
    static class I2CRxFullCallback_t I2CRxQueueFullCallback;
    static class I2CRxDoneCallback_t I2CRxDoneCallback;
#endif
    uint8_t RepeatedStart;
   
    static uint32_t Q_Sucess,Q_Fail;
    int16_t curr;
    uint16_t config;
    uint8_t testID ;  
    uint8_t reg;
    uint16_t value;
    uint16_t slaveaddress = 0x08<<1;
    uint8_t name[] = "Amit Chaudhary is a good Boy";
    name[sizeof(name)/sizeof(uint8_t)-1] = '\n';
    INA219_Dev.HwInit();
    
    B13.HwInit();
#if I2C_INT
    Transaction.XferDoneCallback = &I2C_XferDone_Callback;
    I2CDevIntr.SetCallback(HAL::I2c::I2C_RX_QUEUE_FULL_CALLBACK,&I2CRxQueueFullCallback);
    I2CDevIntr.SetCallback(HAL::I2c::I2C_SLAVE_RX_COMPLETE_CALLBACK,&I2CRxDoneCallback);
    //I2CDevIntr.StartListening();
#endif    
    testID = 15;
    
    while(1)
    {
        switch(testID)
        {
            
#if I2C_INT
        case 0:  // Send 16 bytes
            I2CDevIntr.MasterTx_Intr(slaveaddress,name,sizeof(name)/sizeof(uint8_t),&Status); 
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            testID = 16;
            break;             
        case 1:  // Receive 6 bytes
            I2CDevIntr.MasterRx_Intr(slaveaddress,Intr_TxRx,6,&Status); 
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            testID = 6;            
            break;                
        case 2:  // Receive 3 bytes
            I2CDevIntr.MasterRx_Intr(slaveaddress,Intr_TxRx,3,&Status); 
            break;            
        case 3:  // Receive 2 bytes
            I2CDevIntr.MasterRx_Intr(slaveaddress,Intr_TxRx,2,&Status); 
            break;
        case 4:  // Receive 1 bytes
            I2CDevIntr.MasterRx_Intr(slaveaddress,Intr_TxRx,1,&Status); 
            break;
        case 5:  // Ina219
            reg = 5;
            value = 0x2800;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            I2CDevIntr.MasterTx_Intr(0x80,Intr_TxRx,3,&Status); 
            break;
        case 6:              
            // Set Calibration register to 'Cal' calculated above
            // Write three bytes
            reg = 5;
            value = 4096;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            I2CDevIntr.MasterTx_Intr(0x80,Intr_TxRx,3,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            
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
            I2CDevIntr.MasterTx_Intr(0x80,Intr_TxRx,3,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            testID = 13;
            break;
            
        case 7:    
            // INA219 Test with repeated start 
            // Get Voltage
            reg = 2;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTxRx_Intr(0x80,&Intr_TxRx[0],1,&Intr_TxRx[5],2,true,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            curr = ((Intr_TxRx[5] << 8) | Intr_TxRx[6]);
            Voltage = (int16_t)((curr >> 3) * 4);
            Voltage = Voltage * 0.001;
            
            /* Calibration value*/
            reg = 5;
            value = 4096;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            I2CDevIntr.MasterTx_Intr(0x80,Intr_TxRx,3,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            
            /* Get Current */ 
            reg = 4;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTxRx_Intr(0x80,&Intr_TxRx[0],1,&Intr_TxRx[5],2,1,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            curr = ((Intr_TxRx[5] << 8) | Intr_TxRx[6]);
            Current = curr;  
            Current = Current/10;
            testID = 11;
            break;
            
        case 8:    
            /* INA219 Test without repeated start*/
            /* Get Voltage */ 
            reg = 2;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTxRx_Intr(0x80,&Intr_TxRx[0],1,&Intr_TxRx[5],2,0,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            curr = ((Intr_TxRx[5] << 8) | Intr_TxRx[6]);
            Voltage = (int16_t)((curr >> 3) * 4);
            Voltage = Voltage * 0.001;
            
            /* Calibration value*/
            reg = 5;
            value = 4096;
            Intr_TxRx[0] = reg;
            Intr_TxRx[1] = ((value >> 8) & 0xFF);
            Intr_TxRx[2] = (value & 0xFF);
            I2CDevIntr.MasterTx_Intr(0x80,Intr_TxRx,3,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            
            /* Get Current */ 
            reg = 4;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTxRx_Intr(0x80,&Intr_TxRx[0],1,&Intr_TxRx[5],2,0,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
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
            I2CDevIntr.MasterTxRx_Intr(slaveaddress,&Intr_TxRx[0],4,&Intr_TxRx[5],6,1,&Status); 
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
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
            I2CDevIntr.MasterTxRx_Intr(slaveaddress,&Intr_TxRx[0],4,&Intr_TxRx[5],2,0,&Status); 
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
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
                
            I2CDevIntr.MasterTxRx_Intr(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
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
                
            I2CDevIntr.MasterTx_Intr(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            
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
                
            I2CDevIntr.MasterTxRx_Intr(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            curr = ((Intr_TxRx[5] << 8) | Intr_TxRx[6]);
            Current = curr/10; 
             testID = 15;
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
            
            I2CDevIntr.MasterTx_Intr(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            
            Transaction.SlaveAddress = 0x10;
            Transaction.TxBuf = &Intr_TxRx[0];
            Transaction.TxLen = 0;
            Transaction.RxBuf = &Intr_TxRx[5];
            Transaction.RxLen = 6;
            Transaction.RepeatedStart = 0;
            Transaction.pStatus = &Status;
            
            I2CDevIntr.MasterRx_Intr(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            
            Transaction.SlaveAddress = 0x10;
            Transaction.TxBuf = &Intr_TxRx[0];
            Transaction.TxLen = 4;
            Transaction.RxBuf = &Intr_TxRx[5];
            Transaction.RxLen = 6;
            Transaction.RepeatedStart = RepeatedStart;
            Transaction.pStatus = &Status;
            
            I2CDevIntr.MasterTxRx_Intr(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
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
            
            if ((Status == I2c::I2C_TXN_POSTED) || ( Status == I2c::I2C_OK ) )
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
            if ((Status == I2c::I2C_TXN_POSTED) || ( Status == I2c::I2C_OK ) )
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
            if ((Status == I2c::I2C_TXN_POSTED) || ( Status == I2c::I2C_OK ) )
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
            
            if ((Status == I2c::I2C_TXN_POSTED) || ( Status == I2c::I2C_OK ) )
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
            if ((Status == I2c::I2C_TXN_POSTED) || ( Status == I2c::I2C_OK ) )
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
            if ((Status == I2c::I2C_TXN_POSTED) || ( Status == I2c::I2C_OK ) )
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
#endif        
#if I2C_DMA
            case 15: // INA219 with DMA   
            /* INA219 Test without repeated start*/                
            reg = 2;
            Intr_TxRx[0] = reg;
            I2CDevIntr.MasterTx_DMA(0x80,&Intr_TxRx[0],1,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            
            I2CDevIntr.MasterRx_DMA(0x80,&Intr_TxRx[5],2,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            
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
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            
            /* Get Current */ 
            reg = 4;
            Intr_TxRx[0] = reg;           
            I2CDevIntr.MasterTx_DMA(0x80,&Intr_TxRx[0],1,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            
            I2CDevIntr.MasterRx_DMA(0x80,&Intr_TxRx[5],2,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            curr = ((Intr_TxRx[5] << 8) | Intr_TxRx[6]);
            Current = curr;  
            Current = Current/10;
            //LL_mDelay(100);
            testID = 18;
            break;
            
        case 16:  // Send 16 bytes using DMA
            I2CDevIntr.MasterTx_DMA(slaveaddress,name,sizeof(name)/sizeof(uint8_t),&Status); 
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            //LL_mDelay(1000);
            break;  
        case 17:  // Read 6 bytes using DMA, Rx of 1 byte doesn't work
            I2CDevIntr.MasterRx_DMA(slaveaddress,Intr_TxRx,6,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            break; 
            
        case 18:  // Send 16 bytes using DMA and Read 6 bytes using DMA
            I2CDevIntr.MasterTx_DMA(slaveaddress,name,sizeof(name)/sizeof(uint8_t),&Status); 
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            LL_mDelay(10);
            I2CDevIntr.MasterRx_DMA(slaveaddress,&Intr_TxRx[8],6,&Status);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            testID = 15;
            break;    
        case 19:  // Send 16 bytes using DMA and Read 6 bytes using DMA TxRx with repeated start
            I2CDevIntr.MasterTxRx_DMA(slaveaddress,name,sizeof(name)/sizeof(uint8_t),&Intr_TxRx[7],6,1,&Status); 
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            //LL_mDelay(150);
            testID = 20;  
            break;
        case 20:  
            // Txn using DMA
            Transaction.SlaveAddress = slaveaddress;
            Transaction.TxBuf = name;
            Transaction.TxLen = sizeof(name)/sizeof(uint8_t);
            Transaction.RxBuf = &Intr_TxRx[7];
            Transaction.RxLen = 6;
            Transaction.RepeatedStart = 1;
            Transaction.XferDoneCallback = &I2C_XferDone_Callback;
            Transaction.pStatus = &Status;
            I2CDevIntr.MasterTxRx_DMA(&Transaction);
            while(I2CDevIntr.GetState() != HAL::I2c::READY);
            LL_mDelay(150);
            testID = 21; 
            break;
            
            case 21:  
            // Txn using Post Queue
            Transaction.SlaveAddress = slaveaddress;
            Transaction.TxBuf = name;
            Transaction.TxLen = sizeof(name)/sizeof(uint8_t);
            Transaction.RxBuf = &Intr_TxRx[7];
            Transaction.RxLen = 6;
            Transaction.RepeatedStart = 1;
            Transaction.XferDoneCallback = &I2C_XferDone_Callback;
            Transaction.pStatus = &Status;
            
            I2CDevIntr.Post(&Transaction,1);
                
            Transaction1.SlaveAddress = slaveaddress;
            Transaction1.TxBuf = name;
            Transaction1.TxLen = sizeof(name)/sizeof(uint8_t);
            Transaction1.RxBuf = &Intr_TxRx[7];
            Transaction1.RxLen = 6;
            Transaction1.RepeatedStart = 1;
            Transaction1.XferDoneCallback = &I2C_XferDone_Callback;
            Transaction1.pStatus = &Status;
            
            I2CDevIntr.Post(&Transaction1,1);
                
            Transaction2.SlaveAddress = slaveaddress;
            Transaction2.TxBuf = name;
            Transaction2.TxLen = sizeof(name)/sizeof(uint8_t);
            Transaction2.RxBuf = &Intr_TxRx[7];
            Transaction2.RxLen = 6;
            Transaction2.RepeatedStart = 1;
            Transaction2.XferDoneCallback = &I2C_XferDone_Callback;
            Transaction2.pStatus = &Status;
            
            I2CDevIntr.Post(&Transaction,1);
            I2CDevIntr.Post(&Transaction1,1);
            I2CDevIntr.Post(&Transaction2,1);
            
            I2CDevIntr.Post(&Transaction,1);
            I2CDevIntr.Post(&Transaction1,1);
            I2CDevIntr.Post(&Transaction2,1);
            
            //LL_mDelay(50);
            //testID = 15;             
            break;
#endif           
        default: break;
        
        }
        
        LL_mDelay(5);
    }
}