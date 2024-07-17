//
// Created by xtx on 2023/7/14.
//

#ifndef F407ZGT6_LEDTEST_SRC_TJCSCREEN_H_
#define F407ZGT6_LEDTEST_SRC_TJCSCREEN_H_

#include "ch32l103.h"

void TJCScreenInit(USART_TypeDef *huart);

void TCJSendEnd();

void TCJSendValue(char *name, int value);

void TCJSendTxt(char *name, char *value);

void TCJSetPic(char *name, int id);

void TCJSendAnyProperty(char *object_name, char *property, char *value);

void TCJSendAny(char *any);

#endif //F407ZGT6_LEDTEST_SRC_TJCSCREEN_H_
