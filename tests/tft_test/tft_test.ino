/* 作者：理空电CPSOe
   许可证：MIT
*/

#include <TFT_eSPI.h>
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup()
{
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(0);

  tft.setCursor(5, 5, 1);
  int DarkBlue = tft.color565(48, 64, 82);
  tft.setTextColor(DarkBlue);
  tft.setTextSize(1);
  tft.drawString("OeScreen", 5, 5);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
