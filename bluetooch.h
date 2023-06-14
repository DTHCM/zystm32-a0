#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

/*
前进  按下发出 ONA  松开ONF
后退：按下发出 ONB  松开ONF
左转：按下发出 ONC  松开ONF
右转：按下发出 OND  松开ONF
停止：按下发出 ONE  松开ONF

数字1：按下发出 ON1  松开 ONa
数字2：按下发出 ON2  松开 ONb
数字3：按下发出 ON3  松开 ONc
数字4：按下发出 ON4  松开 ONd
数字5：按下发出 ON5  松开 ONe
数字6：按下发出 ON6  松开 ONf
数字7：按下发出 ON7  松开 ONg
数字8：按下发出 ON8  松开 ONh
数字9：按下发出 ON9  松开 ONi
*/

enum bt_stat {
        BT_CONNECTED, BT_DISCONNECTED,
};

extern volatile int bt_received;
extern volatile int bt_transfered;

void bt_setup(void);
void bt_dma_read(char *data, int size);
void bt_dma_write(char *data, int size);

#endif
