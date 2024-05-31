//
// Created by Fir on 2024/3/7 007.
//

//todo warning flash used age >= 90.93%, will be damaged soon

#include <vector>
#include "astra_rocket.h"
#include "astra/astra_logo.h"

auto *astraLauncher = new astra::Launcher();
auto *rootPage = new astra::Tile("root");

bool test = false;
unsigned char testIndex = 0;
unsigned char testSlider = 60;

auto *sysCfgPage = new astra::List("系统设置");

void astraCoreInit(void) {
  HAL::inject(new ESPHAL);

  HAL::delay(350);
  astra::drawLogo(100);

  HAL::setFont(astra::getUIConfig().mainFont);

  rootPage->addItem(new astra::List("这是什么"));
  rootPage->addItem(new astra::List("页面2"));
  rootPage->addItem(new astra::List("芝士页面3"));
  rootPage->addItem(new astra::List("P3"));
  rootPage->addItem(sysCfgPage);

  sysCfgPage->addItem(new astra::List());
  sysCfgPage->addItem(new astra::List("-使用亮模式2"), new astra::CheckBox(test));
  sysCfgPage->addItem(new astra::Tile("-弹窗3"), new astra::PopUp(1, "测试", {"测试"}, testIndex));
  sysCfgPage->addItem(new astra::Tile("-滑动条4"), new astra::Slider("测试", 0, 100, 50, testSlider));
  sysCfgPage->addItem(new astra::List("-项目5"));
  sysCfgPage->addItem(new astra::List("-这是一个6"));
  sysCfgPage->addItem(new astra::List("-一个一个喜爱那个木6"));
  sysCfgPage->addItem(new astra::List("-一个一个一个6"));
  sysCfgPage->addItem(new astra::List("-一个一个项目6"));

  astraLauncher->init(rootPage);
}

void astraCoreStart(void *pvParas) {
  for (;;) {  //NOLINT
    astraLauncher->update();
//    astraCoreTest();
  }
}

void astraCoreDestroy(void) {
  HAL::destroy();
  delete astraLauncher;
}
