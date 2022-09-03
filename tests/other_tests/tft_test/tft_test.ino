/* 作者：理空电CPSOe
   许可证：MIT
*/

#include <TFT_eSPI.h>
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup()
{
  int nameBlue = tft.color565(48, 64, 82);
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(0);

  tft.setCursor(0, 0, 1);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("OeScreen", 5, 5);
  tft.setTextColor(nameBlue);
  tft.drawString("LiKongDian", 5, 30);
  tft.drawString("CPSOe", 5, 50);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
