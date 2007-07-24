// MonthData.cpp : クラス実装ファイル　（今月、先月の計算をするクラス）
//

// ***********************
// ExecAnalog  （今月および先月のIISデータをAnalogで解析する自動化プログラム）
// 
// Copyright (C) 2007 INOUE. Hirokazu
//
// 
// http://www.opensource.jp/gpl/gpl.ja.html
// このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフトウェ
// ア財団によって発行された GNU 一般公衆利用許諾契約書(バージョン2か、希
// 望によってはそれ以降のバージョンのうちどれか)の定める条件の下で再頒布
// または改変することができます。
// 
// このプログラムは有用であることを願って頒布されますが、*全くの無保証* 
// です。商業可能性の保証や特定の目的への適合性は、言外に示されたものも含
// め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご覧ください。
// 
// あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を一部
// 受け取ったはずです。もし受け取っていなければ、フリーソフトウェア財団ま
// で請求してください(宛先は the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA)。

#include "StdAfx.h"
#include ".\monthdata.h"

CMonthData::CMonthData(void)
{
	// 現在時間を time_t構造体に格納
	::time(&tmNowUx);
}

CMonthData::~CMonthData(void)
{
}

int CMonthData::GetMonthNow(void)
{
	struct tm *tmNow;

	// 現在時間を t 構造体に代入
	tmNow = ::localtime(&tmNowUx);

	// 0,1,2... の補正をして、今月の数を返す
	return tmNow->tm_mon + 1;
}

int CMonthData::GetYearNow(void)
{
	struct tm *tmNow;

	// 現在時間を t 構造体に代入
	tmNow = ::localtime(&tmNowUx);

	// 1900年基点の補正をして、今年の数を返す
	return 1900 + tmNow->tm_year;
}

int CMonthData::GetMonthPrev(void)
{
	struct tm *tmNow;
	int nCurMonth;

	// 現在時間を t 構造体に代入
	tmNow = ::localtime(&tmNowUx);

	// 0,1,2... の補正をして、今月の数を代入
	nCurMonth = tmNow->tm_mon + 1;

	if(nCurMonth > 1) return nCurMonth - 1;
	else return 12;
}

int CMonthData::GetYearPrev(void)
{
	struct tm *tmNow;
	int nCurMonth;

	// 現在時間を t 構造体に代入
	tmNow = ::localtime(&tmNowUx);

	// 0,1,2... の補正をして、今月の数を代入
	nCurMonth = tmNow->tm_mon + 1;

	if(nCurMonth > 1) return 1900 + tmNow->tm_year;
	else return 1900 + tmNow->tm_year - 1;
}
