//----------------------------------------------------------------------------
//    프로그램명 : PHan_Lib.c
//
//    만든이     : Cho Han Cheol 
//
//    날  짜     : 2006.9.18
//    
//    최종 수정  : 2003.9.18
//
//    MPU_Type   : 
//
//    파일명     : PHan_Lib.c
//----------------------------------------------------------------------------

/*


*/


//----- 헤더파일 열기
//
#ifndef   LIB_INCLUDE
#define  PHAN_LIB_LOCAL  
#define  PHANFONT_LOCAL
#define  PHANFONTENG_LOCAL
#include "PHan_Lib.h"
//#include "PHanFont.h"
#include "PHanFontEng.h"

#include <stdio.h>
#include <string.h>
#endif





//----------------------------------------------------------------------------
//
//          TITLE : PHan_FontLoad
//
//          WORK  : 
//
//----------------------------------------------------------------------------
/*----------------------------------------------------------------------*/
/*  한글 일반 폰트(24x24)를 bTemp1Font[72]에 LOAD한다.          */
/*----------------------------------------------------------------------*/
U16 PHan_FontLoad( char *HanCode, PHAN_FONT_OBJ *FontPtr )   /* 한글 일반 폰트 생성 */
{

  //static declaration 은 속도를 높이기 위한것임.
  //static U16 uChosung, uJoongsung, uJongsung, uChoType, uJooType;
  //static U16 uJonType;

  // 버퍼 초기화
  memset(FontPtr->FontBuffer, 0x00, 32);


  FontPtr->Code_Type = PHAN_NULL_CODE;  
  // 한글코드인지 감별 
  // 
  if( !HanCode[0] || HanCode[0] == 0x0A )   // 문자열 마지막
  {
    FontPtr->Code_Type = PHAN_END_CODE;
    FontPtr->Size_Char = 1;
    return PHAN_END_CODE;
  }
  else                                      // 영문 코드 
  {
    FontPtr->Code_Type = PHAN_ENG_CODE;
    FontPtr->Size_Char = 1;
    PHan_EngFontLoad( HanCode, FontPtr );
    return PHAN_ENG_CODE;
  }


  return FontPtr->Code_Type;

}





//----------------------------------------------------------------------------
//
//          TITLE : PHan_EngFontLoad
//
//          WORK  : 
//
//----------------------------------------------------------------------------
void PHan_EngFontLoad( char *HanCode, PHAN_FONT_OBJ *FontPtr ) 
{
  U16 i;
  char EngCode;

  EngCode = *HanCode;

  EngCode -= 0x20;  // FONT는 스페이스 부터 시작한다.

  for ( i = 0 ; i < 16 ; i++ )
  {
     FontPtr->FontBuffer[ i ] = (char)wEngFon[(int)EngCode][i];
  }
}



/*----------------------------------------------------------------------*/
/*  bTemp1Font[72]에 조합한 한글 일반 폰트를              */
/*  Line Image Buffer로 전송한다.                   */
/*----------------------------------------------------------------------*/
void PHan_MoveHanFont( char *ImageBuffer, PHAN_FONT_OBJ *FontPtr, int Xpos)
{
  int i = 0;
  int j = 0;


  for ( j = 0 ; j < 2 ; j++ )   // 16 x 16 (2 Bytes)
  {
    ImageBuffer[ Xpos*2 + j ] = FontPtr->FontBuffer[i*2 +j];
  }
}







void PHan_DisHanFont(PHAN_FONT_OBJ *FontPtr)
{
  U16 i, j, Loop;
  U16 FontSize = FontPtr->Size_Char;


  for ( i = 0 ; i < 16 ; i++ )    // 16 Lines per Font/Char
  {
    for ( j = 0 ; j < FontSize ; j++ )    // 16 x 16 (2 Bytes)
    {
      for( Loop=0; Loop<8; Loop++ )
      {
        if( FontPtr->FontBuffer[i*FontSize +j] & (0x80>>Loop)) printf("*");
        else                                         printf(" ");
        
      }
    }
    printf("   - \n");
    //printf("%x\n",FontPtr->FontBuffer[i]);
  }
}
