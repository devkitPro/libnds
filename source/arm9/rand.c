unsigned short r256table[256];

void r256init(void) 
{
	int i,j,msb;
	j=42424;
	for(i=0;i<256;i++)
	{
		r256table[i]=(unsigned short)(j=j*65539);
	}
	
	msb=0x8000;
	
	j=0;
	
	for(i=0;i<16;i++) 
	{
		j=i*5+3;
		r256table[j]|=msb;
		r256table[j+1]&=~msb;
		msb>>=1;
	}
}

unsigned short rand(void) 
{
	static short init = 0;
	static unsigned char r256index;

	if(!init)
	{
		init = 1;
		r256init();
	}
	int r;
	r=r256table[(r256index+103)&255]^r256table[r256index];
	r256table[r256index++]=r;
	return r;
}	
