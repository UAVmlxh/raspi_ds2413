#include<stdio.h>
#include<wiringPi.h>
#include<math.h>


#define pin 1    // the pin of DS28E01-100
#define ushort unsigned short
#define uchar  unsigned char

static ushort const CRC16Table[256] = {   
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};
/*******************************************************************************
*   函数名称: CRC16
*   功能描述: 查表法计算CRC16.
*   输入参数: dataIn -- 待校验数据
*             length -- 数据长度
*   返 回 值: 校验值
*******************************************************************************/
ushort CRC16(uchar * dataIn, int length)
{  
    ushort result  = 0;
    ushort tableNo = 0;
	int i;
 
    for(i = 0; i < length; i++)
    {
        tableNo = ((result & 0xff) ^ (dataIn[i] & 0xff));
        result  = ((result >> 8) & 0xff) ^ CRC16Table[tableNo];
    }
 
    return result;  
}

//初始化相关的程序
void DS_Init(void)  
{
    wiringPiSetup();                //初始化树莓派
	pullUpDnControl(pin,PUD_UP);   //电路已经上拉
}
//发送复位脉冲
//接受应答脉冲
//成功返回1 不成功返回0        成功
int Ds_config(void)
{
	unsigned int btime;
	int flag=0;
	if(piHiPri(99) !=0)
	{
		//printf("1\n");
		return 1;
	}
	
	pinMode(pin,OUTPUT);           //输出
	digitalWrite(pin,HIGH);        //空闲状态
	digitalWrite(pin,LOW);         //拉低
	delayMicroseconds(500);        //t_RSTL    480 ~ 640
	
	pinMode(pin,INPUT);            //输入
	delayMicroseconds(15);         //t_PDH      15 ~ 60
	btime=micros();                //得到一个微妙数字
	while(1)
	{
		if(abs(micros()-btime)>285)     //max(t_PDH+t_PDL)-15 总线未拉低
		{
			flag=0;break;
		}
		if(digitalRead(pin)==0)    //产生了应答信号
		{
			break;                 //总线拉低
		}
	}
	btime=micros();
	while(1)
	{
		if(abs(micros()-btime)>420)     //t_RSTH-min(t_PDH)-min(t_PDL) 总线未拉高
		{
			flag=0;break;
		}
		if(digitalRead(pin)==1)    //自动拉高
		{
			flag=1;
			break;
		}
	}
	
	if(flag==1)
	{
		pinMode(pin,OUTPUT);
		digitalWrite(pin,LOW);
		//printf("DS is yingda!!\n");
	}
	piHiPri(0);
	return flag;
}
//写一个bit数据   1
int write_bit_1(void)
{
	if(piHiPri(99) !=0)
	{
		//printf("1\n");
		return 1;
	}
	pinMode(pin,OUTPUT);
	digitalWrite(pin,HIGH);
	digitalWrite(pin,LOW);		   //写数据
	delayMicroseconds(6);   
	pinMode(pin,INPUT);     
	delayMicroseconds(65);
	pinMode(pin,OUTPUT);
	digitalWrite(pin,LOW);			// 拉低总线
	pinMode(pin,INPUT);
	piHiPri(0);
}
//写一个bit数据   0
int write_bit_0(void)
{
	if(piHiPri(99) !=0)
	{
		//printf("1\n");
		return 1;
	}
	pinMode(pin,OUTPUT);
	digitalWrite(pin,HIGH);
	digitalWrite(pin,LOW);		   //写数据
	delayMicroseconds(65);   
	pinMode(pin,INPUT);
	delayMicroseconds(6);
	pinMode(pin,OUTPUT);
	digitalWrite(pin,LOW);		   //拉低总线
	pinMode(pin,INPUT);
	piHiPri(0);
}
//读取一bit的值
uchar read_bit(void)
{
	uchar date;
	int btime;
	if(piHiPri(99) !=0)
	{
		//printf("1\n");
		return 1;
	}
	
	pinMode(pin,OUTPUT);
	digitalWrite(pin,HIGH);
	digitalWrite(pin,LOW);		   //t_RL   5us-15us
	delayMicroseconds(6);
	pinMode(pin,INPUT);
	delayMicroseconds(1);
	date=digitalRead(pin);          //读值
	delayMicroseconds(55);         //t_SLOT  = 6+7+55
	pinMode(pin,OUTPUT);
	digitalWrite(pin,LOW);
	pinMode(pin,INPUT);
	return date;
	piHiPri(0);
}
//写一个字节的数据
int write_byte(unsigned char write_date)
{
	uchar  i;
	if(piHiPri(99) !=0)
	{
		//printf("1\n");
		return 1;
	}
	for(i=0;i<8;i++)
	{
		if(write_date&0x01)
		{
			write_bit_1();
		}
		else
		{
			write_bit_0();
		}
		write_date=write_date>>1;
	}
	piHiPri(0);
}
//读一个字节的数据
uchar read_byte(void)
{
	uchar i;
	uchar read_date=0;
	uchar date=0;
	if(piHiPri(99) !=0)
	{
		//printf("1\n");
		return 1;
	}
	for(i=0;i<8;i++)
	{
		if(read_bit())
		{
			date=1;
		}
		else 
		{
			date=0;
		}
		date=date<<i;
		read_date=read_date|date;
	}
	piHiPri(0);
	return read_date;
}

//查询地址数据
int query(uchar adress)
{
	unsigned int i;
	uchar num;
	adress=adress<<3;
	if(Ds_config() !=1)return 0;
	write_byte(0xcc);
	write_byte(0xf0);
	write_byte(adress);
	write_byte(0x00);

	for(i=0;i<8;i++)
	{	
		num=read_byte();
		printf(" %x ",num);
	}
	printf("\n");

	Ds_config();
}





//写入adress的8个字节
//adress 0-15
//16为特殊功能块
//17为保留
int example(uchar adress)
{
	uchar i,temp;
	uchar write_date;
	ushort crc16;
	uchar num1[11]={0x0f,0x00,0x00,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x08};
	uchar num2[12]={0xaa,0x00,0x00,0x07,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x08};
	ushort answer;
	unsigned int btime;
	
	if(adress>15)adress=15;
	
	adress =adress<<3;
	
	num1[1]=adress;
	num2[1]=adress;
	
	
	
	//写数据到暂存器中
	if(Ds_config() !=1)return 0;
		write_byte(0xcc);
		write_byte(num1[0]);  
		write_byte(num1[1]);  
		write_byte(num1[2]);  
		for(i=3;i<11;i++)
		{
			write_date=num1[i];
			write_byte(write_date);
		}
		temp  = read_byte();
		crc16 = read_byte();
		crc16 = (crc16<<8) | temp;
		crc16 = ~crc16;
		answer=CRC16(num1,11);
		if(answer==crc16)
		{
			//printf("1111111good job!\n");
		}
		else
		{
			printf("gg\n");
			return 0;
		}
		
		
		
		
		
	//读暂存器中的数据状态，是否已经读入
		if(Ds_config() !=1)return 0;
		write_byte(0xcc);
		write_byte(0xaa);
		//授权码
		if(read_byte() !=adress)
		{
			printf("gg\n");
			return 0;
		}
		if(read_byte() !=0)
		{
			printf("gg\n");
			return 0;
		}
		if(read_byte() !=0x07)
		{
			printf("gg\n");
			return 0;
		}
	    //读暂存寄存器中的数据
		for(i=0;i<8;i++)
		{
			temp  = read_byte(); 
		}
		//比较CRC16
		temp  = read_byte();
		crc16 = read_byte();
		crc16 = (crc16<<8) | temp;
		crc16 = ~crc16;
		answer=CRC16(num2,12);
		if(answer==crc16)
		{
			//printf("1111111good job!\n");
		}
		else
		{
			printf("gg\n");
		}
		
		
		//写入对应的地址中
		if(Ds_config() !=1)return 0;
		write_byte(0xcc);
		write_byte(0x55);
		write_byte(adress);
		write_byte(0x00);
		write_byte(0x07);
		

		delay(13);
	    temp=read_byte();
		if(temp == 0xaa)
		{	
			//printf("write success!!\n");
			return 0;
		}
		else
		{
			printf("gg\n");
		}
		/*btime=millis();
		while(abs(millis()-btime)<13)
		{
			temp =read_byte();
			if(temp == 0xaa)
			{	
				printf("write success!!\n");
				return 0;
			}
		}	*/
		
		return 0;
}


int main(void)
{
	unsigned char flag=0;
	unsigned int  i;
	DS_Init();
	//query(0x00);
	//delay(2);
	printf("input  1\n");
	while(1)
	{
		scanf("%x",&flag);
		
		if(flag==1)
		{
			printf("please input your chose!\n");
			scanf("%x",&flag);
			example(flag);
			query(flag);
		}
		if(flag==2)
		{
			for(i=0;i<16;i++)
			{
				delay(1);
				query(i);
			}
		}
		
	}
}





