#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define READ_BINARY_SIZE (1000000) //バイナリファイルを1度に読み込むサイズ
#define MIN_OUTPU_MARK (1000.0)    //結果出力を始めるビット数

FILE *OpenFile(const char *filename, const char *mode)
{
  FILE *fp;

  if(!(fp = fopen(filename, mode)))
	{
	  fprintf(stderr, "Error: fopen (%s)", filename);
	  exit(1);
	}

  return fp;
}

char *GetFilename(FILE *fp_list, char *filename)
{
  char *cp;
  if((cp=fgets(filename, sizeof(char)*1024, fp_list)))
	sscanf(filename, "%s\n", filename); //改行を除去
 
  return cp;
}

unsigned char GetLSB(const unsigned char c, const int lsb)
{
  if(lsb<1||lsb>8)
	{
	  fprintf(stderr, "Error: GetLSB() lsb=%d", lsb);
	  exit(1);
	}

  return (c & 0x01<<(lsb-1))? 1:0;
}


int main(int argc, char** argv)
{
  FILE *fp_list, *fp_binary, *fp_bias;
  char filename_binary[1024];
  char filename_bias[1024];
  unsigned char readBinary[READ_BINARY_SIZE];

  unsigned long int CountMatrix = 0;
  unsigned long int oneCountMatrix = 0;

  double mark_output = 1000.0; //結果出力するビット数

  if(argc != 2)
    {
      fprintf(stderr,"argument error\n");
      exit(1);
    }

  //リストファイルをオープン
  fp_list = OpenFile(argv[1], "r");

  sprintf(filename_bias, "%s_NbitBias.data", argv[1]);
  //結果出力用ファイルをオープン
  fp_bias = OpenFile(filename_bias, "w");


  while( GetFilename(fp_list, filename_binary))
	{
	  fp_binary = OpenFile(filename_binary, "rb");

	  int readed = fread(readBinary, sizeof(unsigned char), READ_BINARY_SIZE, fp_binary);
	  while(readed != 0)
		{
		  for(int i=0; i<readed; i++)
			{
			  for(int lsb=1; lsb<=8; lsb++)
				{
				  if(GetLSB(readBinary[i], lsb)==1)
					oneCountMatrix++;

				  if(++CountMatrix >= mark_output)
					{
					  fprintf(fp_bias,"%lu\t", CountMatrix);
					  fprintf(fp_bias,"%e\t", fabs(0.5 - (double) oneCountMatrix/CountMatrix));
					  fprintf(fp_bias,"%lu\t", oneCountMatrix);
					  fprintf(fp_bias,"%lu\n", CountMatrix);
					  //次の出力するビット数を再設定
					  mark_output = pow(10, log10(mark_output) + 0.1);
					}
				}
			}

		  readed = fread(readBinary, sizeof(unsigned char), READ_BINARY_SIZE, fp_binary);
		}

	  fclose(fp_binary);
	}

  fclose(fp_list);
  fclose(fp_bias);

  return 0;
}
