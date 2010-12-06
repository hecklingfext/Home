#ifndef _BLIT_H_
#define _BLIT_H_

void Blit_X1(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_X2(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_X2_Int(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_Scanline(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_Scanline_Int(unsigned char *Dest, int pitch, int x, int y, int offset);

void Blit_X1_MMX(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_X2_MMX(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_X2_Int_MMX(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_Scanline_MMX(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_Scanline_Int_MMX(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_Scanline_50_MMX(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_Scanline_50_Int_MMX(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_Scanline_25_MMX(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_Scanline_25_Int_MMX(unsigned char *Dest, int pitch, int x, int y, int offset);

void Blit_2xSAI_MMX(unsigned char *Dest, int pitch, int x, int y, int offset);
void Blit_Scale2x(unsigned char *Dest, int pitch, int x, int y, int offset);
void _Blit_HQ2x(unsigned char *Dest, int pitch, int x, int y, int offset);
int Blit_HQ2x_InitLUTs(void);

void Gens_2_RGBA_BYTE (unsigned long *Dest, const unsigned short * Src,  unsigned int size);

#endif
