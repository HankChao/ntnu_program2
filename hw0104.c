#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "xiangqi.h"

int main() {
 sXiangqiRecord *record = initXiangqiRecord();
 if (record == NULL) {
    printf("初始化失敗\n");
    return -1;
 }
 printf("初始棋盤：\n");
 if(moveXiangqiRecord(record, 0, 4, 1, 3)==-1) printf("紅帥斜走\n");
 // 紅相
 if(moveXiangqiRecord(record, 0, 6, 2, 4)==-1) printf("1.error\n");
 // 黑卒//橫移（未過河）
 if(moveXiangqiRecord(record, 6, 6, 6, 5)==-1) printf("黑卒橫移（未過河)\n");
//  黑卒//後移
 if(moveXiangqiRecord(record, 6, 6, 7, 6)==-1) printf("後移\n");
//  黑炮//走斜
 if(moveXiangqiRecord(record, 2, 7, 1, 6)==-1) printf("黑炮走斜\n");
//  黑車//走斜
 if(moveXiangqiRecord(record, 9, 8, 8, 7)==-1) printf("黑車走斜\n");
//  黑士//直走
 if(moveXiangqiRecord(record, 9, 5, 8, 5)==-1) printf("黑士走直\n");
//  黑炮
 if(moveXiangqiRecord(record, 7, 7, 7, 3)==-1) printf("2.error\n");
 // 紅傌
 if(moveXiangqiRecord(record, 0, 7, 2, 6)==-1) printf("3.error\n");
 // 黑馬
 if(moveXiangqiRecord(record, 9, 7, 7, 6)==-1) printf("4.error\n");
 // 紅炮//飛吃自己人
 if(moveXiangqiRecord(record, 2, 7, 2, 4)==-1) printf("紅炮別吃自己人阿！\n");
 // 紅車
 if(moveXiangqiRecord(record, 0, 8, 0, 7)==-1) printf("5.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 9, 8, 9, 7)==-1) printf("6.error\n");
 // 紅炮
 if(moveXiangqiRecord(record, 2, 7, 6, 7)==-1) printf("7.error\n");
 // 黑卒
 if(moveXiangqiRecord(record, 6, 6, 5, 6)==-1) printf("8.error\n");
 // 紅兵
 if(moveXiangqiRecord(record, 3, 2, 4, 2)==-1) printf("9.error\n");
 // 黑馬
 if(moveXiangqiRecord(record, 9, 1, 7, 0)==-1) printf("10.error\n");
 // 紅馬
 if(moveXiangqiRecord(record, 0, 1, 2, 2)==-1) printf("11.error\n");
 // 黑炮
 if(moveXiangqiRecord(record, 7, 1, 7, 2)==-1) printf("12.error\n");
 // 紅馬
 if(moveXiangqiRecord(record, 2, 2, 4, 1)==-1) printf("13.error\n");
 // 黑馬
 if(moveXiangqiRecord(record, 7, 6, 5, 5)==-1) printf("14.error\n");
 // 紅炮
 if(moveXiangqiRecord(record, 6, 7, 4, 7)==-1) printf("15.error\n");
 // 黑炮
 if(moveXiangqiRecord(record, 7, 3, 7, 4)==-1) printf("16.error\n");
 // 紅仕
 if(moveXiangqiRecord(record, 0, 5, 1, 4)==-1) printf("17.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 9, 0, 8, 0)==-1) printf("18.error\n");
 // 紅馬
 if(moveXiangqiRecord(record, 4, 1, 6, 0)==-1) printf("19.error\n");
 // 黑炮
 if(moveXiangqiRecord(record, 7, 2, 7, 3)==-1) printf("20.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 0, 0, 1, 0)==-1) printf("21.error\n");
 // 黑炮
 if(moveXiangqiRecord(record, 7, 3, 6, 3)==-1) printf("22.error\n");
 // 紅馬
 if(moveXiangqiRecord(record, 6, 0, 4, 1)==-1) printf("23.error\n");
 // 黑炮
 if(moveXiangqiRecord(record, 6, 3, 4, 3)==-1) printf("24.error\n");
 // 紅兵
 if(moveXiangqiRecord(record, 4, 2, 5, 2)==-1) printf("25.error\n");
 // 黑卒
 if(moveXiangqiRecord(record, 6, 2, 5, 2)==-1) printf("26.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 1, 0, 1, 2)==-1) printf("27.error\n");
 // 黑馬
 if(moveXiangqiRecord(record, 5, 5, 3, 6)==-1) printf("28.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 1, 2, 5, 2)==-1) printf("29.error\n");
 // 黑炮
 if(moveXiangqiRecord(record, 4, 3, 1, 3)==-1) printf("30.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 5, 2, 5, 3)==-1) printf("31.error\n");
 // 黑馬
 if(moveXiangqiRecord(record, 3, 6, 2, 4)==-1) printf("32.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 5, 3, 1, 3)==-1) printf("33.error\n");
 // 黑馬//將軍
 if(moveXiangqiRecord(record, 2, 4, 1, 6)==-1) printf("34.error\n");
 // 紅帥
 if(moveXiangqiRecord(record, 0, 4, 0, 5)==-1) printf("35.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 8, 0, 8, 5)==-1) printf("36.error\n");
 // 紅炮
 if(moveXiangqiRecord(record, 2, 1, 2, 5)==-1) printf("37.error\n");
 // 黑卒
 if(moveXiangqiRecord(record, 5, 6, 4, 6)==-1) printf("38.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 0, 7, 0, 6)==-1) printf("39.error\n");
 // 黑卒
 if(moveXiangqiRecord(record, 4, 6, 3, 6)==-1) printf("40.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 0, 6, 1, 6)==-1) printf("41.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 9, 7, 4, 7)==-1) printf("42.error\n");
 // 紅馬
 if(moveXiangqiRecord(record, 4, 1, 5, 3)==-1) printf("43.error\n");
 // 黑炮
 if(moveXiangqiRecord(record, 7, 4, 7, 6)==-1) printf("44.error\n");
 // 紅馬
 if(moveXiangqiRecord(record, 5, 3, 7, 2)==-1) printf("45.error\n");
 // 黑象//拐象眼
 if(moveXiangqiRecord(record, 9, 6, 7, 4)==-1) printf("黑象拐象眼\n");
 // 黑士
 if(moveXiangqiRecord(record, 9, 5, 8, 4)==-1) printf("46.error\n");
 // 紅帥
 if(moveXiangqiRecord(record, 0, 5, 0, 4)==-1) printf("47.error\n");
 // 黑炮
 if(moveXiangqiRecord(record, 7, 6, 2, 6)==-1) printf("48.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 1, 3, 6, 3)==-1) printf("49.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 8, 5, 7, 5)==-1) printf("50.error\n");
 // 紅馬
 if(moveXiangqiRecord(record, 7, 2, 6, 4)==-1) printf("51.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 7, 5, 6, 5)==-1) printf("52.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 1, 6, 0, 6)==-1) printf("53.error\n");
 // 黑馬
 if(moveXiangqiRecord(record, 7, 0, 5, 1)==-1) printf("54.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 6, 3, 6, 2)==-1) printf("55.error\n");
 // 黑象
 if(moveXiangqiRecord(record, 9, 6, 7, 4)==-1) printf("56.error\n");
 // 紅炮
 if(moveXiangqiRecord(record, 2, 5, 2, 4)==-1) printf("57.error\n");
 // 黑卒
 if(moveXiangqiRecord(record, 3, 6, 3, 5)==-1) printf("58.error\n");
 // 紅兵
 if(moveXiangqiRecord(record, 3, 4, 4, 4)==-1) printf("59.error\n");
 // 黑卒
 if(moveXiangqiRecord(record, 3, 5, 3, 4)==-1) printf("60.error\n");
 // 紅炮
 if(moveXiangqiRecord(record, 2, 4, 2, 5)==-1) printf("61.error\n");
 // 黑炮
 if(moveXiangqiRecord(record, 2, 6, 6, 6)==-1) printf("62.error\n");
 // 紅兵
 if(moveXiangqiRecord(record, 4, 4, 5, 4)==-1) printf("63.error\n");
 // 黑炮
 if(moveXiangqiRecord(record, 6, 6, 4, 6)==-1) printf("64.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 6, 2, 4, 2)==-1) printf("65.error\n");
 // 黑炮
 if(moveXiangqiRecord(record, 4, 6, 4, 4)==-1) printf("66.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 0, 6, 0, 5)==-1) printf("67.error\n");
 // 黑馬
 if(moveXiangqiRecord(record, 5, 1, 3, 0)==-1) printf("68.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 4, 2, 4, 0)==-1) printf("69.error\n");
 // 黑馬
 if(moveXiangqiRecord(record, 3, 0, 1, 1)==-1) printf("70.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 4, 0, 4, 3)==-1) printf("71.error\n");
 // 黑卒
 if(moveXiangqiRecord(record, 3, 4, 3, 3)==-1) printf("72.error\n");
 // 紅相
 if(moveXiangqiRecord(record, 0, 2, 2, 4)==-1) printf("73.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 6, 5, 2, 5)==-1) printf("74.error\n");
 // 紅車
 if(moveXiangqiRecord(record, 4, 3, 4, 4)==-1) printf("75.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 2, 5, 0, 5)==-1) printf("76.error\n");
 // 紅帥
 if(moveXiangqiRecord(record, 0, 4, 0, 5)==-1) printf("77.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 4, 7, 4, 4)==-1) printf("78.error\n");
 // 紅馬//拐馬腳
 if(moveXiangqiRecord(record, 6, 4, 8, 5)==-1) printf("紅馬拐馬腳\n");
 // 紅馬
 if(moveXiangqiRecord(record, 6, 4, 7, 6)==-1) printf("79.error\n");
 // 黑將//自殺
 if(moveXiangqiRecord(record, 9, 4, 9, 5)==-1) printf("黑將自殺\n");
 // 黑車
 if(moveXiangqiRecord(record, 4, 4, 4, 5)==-1) printf("80.error\n");
 // 紅帥//自殺
 if(moveXiangqiRecord(record, 0, 5, 1, 5)==-1) printf("紅帥自殺\n");
 // 紅帥
 if(moveXiangqiRecord(record, 0, 5, 0, 4)==-1) printf("81.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 4, 5, 4, 7)==-1) printf("82.error\n");
 // 紅馬
 if(moveXiangqiRecord(record, 7, 6, 6, 8)==-1) printf("83.error\n");
 // 黑將
 if(moveXiangqiRecord(record, 9, 4, 9, 5)==-1) printf("84.error\n");
 // 紅兵//橫移（未過河）
 if(moveXiangqiRecord(record, 3, 8, 3, 7)==-1) printf("橫移（未過河\n");
 // 紅兵//後移
 if(moveXiangqiRecord(record, 3, 8, 2, 8)==-1) printf("紅兵後移（未過河\n");
 // 紅士
 if(moveXiangqiRecord(record, 1, 4, 2, 3)==-1) printf("85.error\n");
 // 黑卒
 if(moveXiangqiRecord(record, 3, 3, 2, 3)==-1) printf("86.error\n");
 // 紅士
 if(moveXiangqiRecord(record, 0, 3, 1, 4)==-1) printf("87.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 4, 7, 0, 7)==-1) printf("88.error\n");
 // 紅象
 if(moveXiangqiRecord(record, 2, 4, 0, 6)==-1) printf("89.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 0, 7, 0, 6)==-1) printf("90.error\n");
 // 紅士
 if(moveXiangqiRecord(record, 1, 4, 0, 5)==-1) printf("91.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 0, 6, 0, 5)==-1) printf("92.error\n");
 // 紅帥
 if(moveXiangqiRecord(record, 0, 4, 1, 4)==-1) printf("93.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 0, 5, 1, 5)==-1) printf("94.error\n");
 // 紅帥
 if(moveXiangqiRecord(record, 1, 4, 0, 4)==-1) printf("95.error\n");
 // 黑卒
 if(moveXiangqiRecord(record, 2, 3, 1, 3)==-1) printf("96.error\n");
 // 紅馬
 if(moveXiangqiRecord(record, 6, 8, 4, 7)==-1) printf("97.error\n");
 // 黑車
 if(moveXiangqiRecord(record, 1, 5, 0, 5)==-1) printf("98.error\n");
 // 紅帥//王見王
 if(moveXiangqiRecord(record, 0, 4, 0, 5)==-1) printf("紅帥王見王\n");
 printf("\n黑方勝\n");
 printf("最終棋盤：\n");


 printXiangqiPlay(record);
 freeXiangqiRecord(record);

 return 0;

}
