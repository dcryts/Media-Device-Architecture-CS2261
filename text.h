/*********************************************************************
 *
 * text.h
 *
 ********************************************************************/
#ifndef TEXT_H
#define TEXT_H

void drawChar(int , int , char , unsigned short );
void drawString(int row, int col, char *str, unsigned short color);
void drawChar4(int row, int col, char ch, unsigned char colorIndex);

void drawString4(int row, int col, char *str, unsigned char colorIndex);

extern const unsigned char fontdata_6x8[12288];

#endif
