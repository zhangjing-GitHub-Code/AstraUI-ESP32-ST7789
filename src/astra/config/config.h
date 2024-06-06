//
// Created by Fir on 2024/1/25.
//

#pragma once
#ifndef ASTRA_CORE_SRC_SYSTEM_H_
#define ASTRA_CORE_SRC_SYSTEM_H_

#include "U8g2lib.h"
#include "../../hal/hal-esp32-st7789/fonts/font_decl.h"
#include <Arduino_GFX_Library.h>

namespace astra {
/**
 * @brief config of astra ui. astra ui的配置结构体
 */
struct config {
  float tileAnimationSpeed = 90; // 70
  float listAnimationSpeed = 80;
  float selectorYAnimationSpeed = 70;
  float selectorXAnimationSpeed = 80;
  float selectorWidthAnimationSpeed = 70;
  float selectorHeightAnimationSpeed = 55;
  float windowAnimationSpeed = 25; // 25
  float sideBarAnimationSpeed = 15;
  float fadeAnimationSpeed = 100;
  float cameraAnimationSpeed = 90; // 80
  float logoAnimationSpeed = 80; // 70

  bool tileUnfold = true;
  bool listUnfold = true;

  bool menuLoop = false;

  bool backgroundBlur = true;
  bool lightMode = false;

  float listBarWeight = 5;
  float listTextHeight = 16;
  float listTextMargin = 4; //文字边距
  float listLineHeight = 32;
  float selectorRadius = 4;
  float selectorMargin = 4; //选择框与文字左边距
  float selectorTopMargin = 2; //选择框与文字上边距

  float tilePicWidth = 30;
  float tilePicHeight = 30;
  float tilePicMargin = 10; // 8
  float tilePicTopMargin = 18; // 8 //图标上边距
  float tileArrowWidth = 8; // 6
  float tileArrowMargin = 4; //箭头边距

  float tileDottedLineBottomMargin = 18; //虚线下边距(top: 46)
  float tileArrowBottomMargin = 8; //箭头下边距(top: 56)
  float tileTextBottomMargin = 12; //标题下边距(top: 52)

  float tileBarHeight = 2; //磁贴进度条高度

  float tileSelectBoxLineLength = 6;  //磁贴选择框线长
  float tileSelectBoxMargin = 5; //选择框边距
  float tileSelectBoxWidth = tileSelectBoxMargin * 2 + tilePicWidth; //选择框宽
  float tileSelectBoxHeight = tileSelectBoxMargin * 2 + tilePicHeight; //选择框高
  float tileTitleHeight = 8; //磁贴标题高度

  float tileBtnMargin = 16; //按钮边距

  float popMargin = 4; //弹窗边距
  float popRadius = 5; // 2 //弹窗圆角半径
  float popSpeed = 90; //弹窗动画速度

  float logoStarLength = 5; //logo星星长度
  float logoTextHeight = 14; //logo文字高度
  float logoCopyRightHeight = 8; //logo文字高度
  unsigned char logoStarNum = 16; //logo星星数量

  const unsigned char *logoTitleFont = u8g2_font_Cascadia;
  const unsigned char *logoCopyRightFont = u8g2_font_myfont;
  const unsigned char *mainFont = u8g2_font_unifont_t_chinese3;

  //如果未使用u8g2库 请自行修改这里
  //const unsigned char *logoTitleFont = u8g2_font_Cascadia;
  //const unsigned char *logoCopyRightFont = u8g2_font_myfont;
  //const unsigned char *mainFont = u8g2_font_myfont;

  float checkBoxWidth = 8;
  float checkBoxHeight = 8;
  float checkBoxTopMargin = 4; //与选项上边缘的距离
  float checkBoxRightMargin = 10; //与屏幕右边缘的距离
  float checkBoxRadius = 5; // 4 // 1
};

static config &getUIConfig() {
  static config astraConfig;
  return astraConfig;
}
}
#endif //ASTRA_CORE_SRC_SYSTEM_H_