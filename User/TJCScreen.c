//
// Created by xtx on 2023/7/14.
//

#include <string.h>
#include <stdio.h>
#include "uart.h"
#include "TJCScreen.h"

USART_TypeDef *gHuart;

void TJCScreenInit(USART_TypeDef *huart) {
    gHuart = huart;
    TCJSendEnd();
}

void TCJSendEnd() {
    USART_Send(gHuart, (uint8_t *) "\xff\xff\xff", 3);
}
void TCJSetPic(char *name, int id) {
    char tmp[30];
    sprintf(tmp, "%s.pic=%d", name, id);
    USART_Send(gHuart, (uint8_t *) tmp, strlen(tmp));
    TCJSendEnd();
}

void TCJSendValue(char *name, int value) {
    char tmp[30];
    sprintf(tmp, "%s.val=%d", name, value);
    USART_Send(gHuart, (uint8_t *) tmp, strlen(tmp));
    TCJSendEnd();
}

void TCJSendTxt(char *name, char *value) {
    char tmp[30];
    sprintf(tmp, "%s.txt=\"%s\"", name, value);
    USART_Send(gHuart, (uint8_t *) tmp, strlen(tmp));
    TCJSendEnd();
}

void TCJSendAnyProperty(char *object_name, char *property, char *value) {
    char tmp[30];
    sprintf(tmp, "%s.%s=%s", object_name, property, value);
    USART_Send(gHuart, (uint8_t *) tmp, strlen(tmp));
    TCJSendEnd();
}

void TCJSendAny(char *any) {
    USART_Send(gHuart, (uint8_t *) any, strlen(any));
    TCJSendEnd();
}