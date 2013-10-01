#ifndef _SHOWBM_H_
#define _SHOWBM_H_

#define NOFILEDAYS	30      /* 本板无新文天数, >NOFILEDAYS以红色显示 */
#define NO0FILEDAYS	30      /* 精华区未整理天数, >NO0FILEDAYS以红色显示 */
#define BMNOLOGIN	30      /* 最近未上站天数, >BMNOLOGIN以红色显示 */
#define BMNOFILEDAYS	30      /* BM本板未发文天数, >BMNOFILEDAYS以红色显示 */
#define DAYSN		7       /* 修改本数字请同步修改下面head描述 */

/* 如果有必要修改以下数据, 请注意空格的数目 */
const char *head =
    "----------------- ----- ------ ------ ------ ------------ ------ ------ ------
    板 名            本周  本板  精华区 精华区   板 主       本板   最近  BM本板
    板上 无新文  文章  未整理    或         板主  未上站 未发文
    文章数 天数   总数   天数    板 副      文章数  天数   天数
    ----------------- ----- ------ ------ ------ ------------ ------ ------ ------\n";

#endif
