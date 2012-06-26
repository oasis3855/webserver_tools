#!/usr/bin/perl

# save this file in << UTF-8  >> encode !
# ******************************************************
# Software name : アクセス統計HTML作成スクリプト for Webページ用 カウンタCGI
#
# Copyright (C) INOUE Hirokazu, All Rights Reserved
#     http://oasis.halfmoon.jp/
#
# stat_monthly.pl
#
# version 3.0, 2011/03/26  utf8化
# version 3.0.1, 2012/06/24  Google Chart Tool API verup
# version 3.1, 2012/06/26  ApacheLog解析のDBより、総アクセス数を得る
#
# GNU GPL Free Software
#
# このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフトウェア財
# 団によって発行された GNU 一般公衆利用許諾契約書(バージョン2か、希望によっては
# それ以降のバージョンのうちどれか)の定める条件の下で再頒布または改変することが
# できます。
#
# このプログラムは有用であることを願って頒布されますが、*全くの無保証* です。
# 商業可能性の保証や特定の目的への適合性は、言外に示されたものも含め全く存在し
# ません。詳しくはGNU 一般公衆利用許諾契約書をご覧ください。
#
# あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を一部受け取
# ったはずです。もし受け取っていなければ、フリーソフトウェア財団まで請求してく
# ださい(宛先は the Free Software Foundation, Inc., 59 Temple Place, Suite 330
# , Boston, MA 02111-1307 USA)。
#
# http://www.opensource.jp/gpl/gpl.ja.html
# ******************************************************
# *************
# 使用法
#   stat_monthly.pl			CUIによる対話的入力
#   stat_monthly [1 .. 12]	本日を起点として1から12ヶ月を対象として自動実行
# *************

use warnings;
use strict;
use utf8;
use DBI qw(:sql_types);
use Time::Local;
use URI::Escape;
use Encode::Guess qw/euc-jp shiftjis iso-2022-jp/;	  # 必要ないエンコードは削除すること
use FindBin qw/$Bin/;	# サーバ上でのフルパス名を得るため
use Time::HiRes;	# ミリ秒を利用する（処理時間計測用）

# 文字コード変換 共通サブルーチン用定義
my $flag_os = 'linux';  # linux/windows
my $flag_charcode = 'utf8';             # utf8/shiftjis
# IOの文字コードを規定
if($flag_charcode eq 'utf8'){
	binmode(STDIN, ":utf8");
	binmode(STDOUT, ":utf8");
	binmode(STDERR, ":utf8");
}
if($flag_charcode eq 'shiftjis'){
	binmode(STDIN, "encoding(sjis)");
	binmode(STDOUT, "encoding(sjis)");
	binmode(STDERR, "encoding(sjis)");
}

# サーバ環境に合わせた設定（グローバル変数に読み込む）
require '../include/setup.pl';
our $strApachelogSqlDsn;	# 日毎アクセス数統計にApacheLogを用いる場合のDSN文字列

# SQLサーバのDSN定義
my $strSqlDsn = 'DBI:SQLite:dbname='.$FindBin::Bin.'/../data/';
my $strDefaultDB = 'accdb.sqlite';

# 結果を書き込むファイル名
my $str_filepath_out = $FindBin::Bin.'/../stat.html';

# 集計する月数
my $n_stat_month = 6;

# 環境変数TZを日本時間に設定する
$ENV{'TZ'} = "JST-9";

# 開始年月日（UNIX時刻）
my $tm_start = time();

# クエリにかかる時間を計測する
my $tm_lapse_query = undef;		# クエリにかかった時間
my $tm_lapse_query_start = undef;	# クエリ開始時刻

# 変数
my $nTotalData = 0;		# データベース全体での全データ数
my @arr_nSubTotalData = ();	# 12ヶ月の1ヶ月ごとのデータ数
my @arr_nDayTotalData = ();	# 直近30日間の1日毎のデータ数
my @arr_nDayTotalDataAll = ();	# 直近30日間の1日毎のデータ数（ApacheLogを用いた全アクセス数）
my @arr_BrowserStatData = ();	# 統計データ（ブラウザ）
my @arr_OsStatData = ();	# 統計データ（OS）
my @arr_PopularPage = ();	# 統計データ（アクセス数の多いページ順）
my $nBrowserTopic = 0;	# ブラウザ統計項目数
my $nOsTopic = 0;	# OS統計項目数
my $nPopularPages = 0;

# 出力HTML用定義
my $strHtmlBackgroundColor = '#f3f3f3';
my $strHtmlH1LineColor = '#7e7e7e';
my $strHtmlH2LineColor = '#a7a7a7';
my $strHtmlTableGridColor = '#7e7e7e';

# 一時変数

my $strTmp = "";
my $strDateTime = "";
my $nStartEpockSec = 0;
my $nEndEpockSec = 0;
my @arrayWdayStr = ("Sun","Mon","Tue","Wed","Thu","Fri","Sat");

my $i = 0;
my $j = 0;

my @row = ();		# クエリ結果を受ける配列



# 引数（対象年）の処理。またはコンソールから対象年、DB名を読み込む
if($#ARGV != 0) {
	# プログラムへの引数がない場合

	# 開始年月日の対話的入力
	my $str = sprintf("%d/%02d/%02d", (localtime($tm_start))[5]+1900, (localtime($tm_start))[4]+1, (localtime($tm_start))[3]);
	print("input start yyyy/mm/dd (default [".$str."]) : ");
	$_ = <>;
	chomp();
	if($_ eq '') { 
		$tm_start = timelocal(59,59,23,(localtime(time()))[3],(localtime(time()))[4],(localtime(time()))[5]);
	}
	else
	{
		my @arr_ymd = split(/,|\.|:|-|\//, $_);	# 年月日を配列に分離
		$tm_start = timelocal(59,59,23,$arr_ymd[2],$arr_ymd[1]-1,$arr_ymd[0]-1900);
	}
	# 開始年月日の正当性をチエック
	if($tm_start > timelocal(59,59,23,31,12-1,2032-1900) || $tm_start < timelocal(0,0,0,1,1-1,1970-1900))
	{
		$tm_start = time();
	}
	$str = sprintf("info : start time = %04d/%02d/%02d\n",(localtime($tm_start))[5]+1900, (localtime($tm_start))[4]+1, (localtime($tm_start))[3]);
	print($str);

	# 月数の対話的入力
	print("input target months (default [".$n_stat_month."]) : ");
	$_ = <>;
	chomp();
	if($_ ne '') { $n_stat_month = int($_ + 0); }
	if($n_stat_month < 1 || $n_stat_month > 12) { $n_stat_month = 6; }
	print("info : stat month = ".$n_stat_month."\n");

	# DB名の対話的入力
	print("input target DB (default [".$strDefaultDB."]) : ");
	$_ = <>;
	chomp();
	if($_ eq ''){ $strSqlDsn .= $strDefaultDB; }
	else{ $strSqlDsn .= $strTmp; }
}
else
{
	# 引数があれば、それを対象月数として読み込む
	$n_stat_month = int($ARGV[0] + 0);	# 整数化する
	
	# 標準のDB名を用いる
	$strSqlDsn .= $strDefaultDB;
}

# クエリにかかる時間を計測するため、開始時刻を保存
$tm_lapse_query_start = Time::HiRes::time();

#**************
# ログDBからの統計データの抽出

my $dbh = undef;
eval{

	# SQLサーバに接続
	$dbh = DBI->connect($strSqlDsn, "", "", {PrintError => 1, AutoCommit => 0}) or die(DBI::errstr);

	# データベース全体のデータ数
	my $str_query = "SELECT COUNT (*) FROM acctbl";
	my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
	$sth->execute() or die(DBI::errstr);
	@row = $sth->fetchrow_array();
	$nTotalData = $row[0];

	$sth->finish() or die(DBI::errstr);
	$sth = undef;	# 再利用のため初期化

	print("open DB : ".$strSqlDsn."\n".
			"info : total data ".$nTotalData."\n");

	print("info : monthly total access stat processing ...\n");

	##############################
	# １ヶ月間隔のデータベースのデータ数
	for($i=0; $i<$n_stat_month; $i++)
	{
		$nStartEpockSec = $tm_start - 30*24*60*60*($i+1);
		$nEndEpockSec = $tm_start - 30*24*60*60*$i;
		$str_query = "SELECT COUNT (*) FROM acctbl WHERE tm_epock >= ? AND tm_epock <= ?";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->bind_param(1, $nStartEpockSec, SQL_INTEGER) or die(DBI::errstr);
		$sth->bind_param(2, $nEndEpockSec, SQL_INTEGER) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		@row = $sth->fetchrow_array();
		$arr_nSubTotalData[$i] = $row[0];
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用のため初期化
		
	}

	print("info : daily total access stat processing ...\n");

	##############################
	# 直近 過去30日間の1日毎のアクセス数
	$str_query = "SELECT COUNT (*) FROM acctbl WHERE tm_epock >= ? AND tm_epock <= ?";
	$sth = $dbh->prepare($str_query) or die(DBI::errstr);
	for($i=0; $i<30; $i++)
	{
		$nStartEpockSec = $tm_start - 24*60*60*($i+1);
		$nEndEpockSec = $tm_start - 24*60*60*$i;
		$sth->bind_param(1, $nStartEpockSec, SQL_INTEGER) or die(DBI::errstr);
		$sth->bind_param(2, $nEndEpockSec, SQL_INTEGER) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		@row = $sth->fetchrow_array();
		$arr_nDayTotalData[$i] = $row[0];
	}
	$sth->finish() or die(DBI::errstr);
	$sth = undef;	# 再利用のため初期化
	

	##############################
	# ブラウザ 集計

	print("info : browser stat processing ...\n");

	# ブラウザ集計テーブルを読み込む
	$sth = $dbh->prepare("select * from statbrowsertbl") or die(DBI::errstr);
	$sth->execute() or die(DBI::errstr);

	$nBrowserTopic = 0;
	while(@row = $sth->fetchrow_array())
	{
		$arr_BrowserStatData[$nBrowserTopic][0] = $row[1];	# 検索パターン
		$arr_BrowserStatData[$nBrowserTopic][1] = $row[2];	# ブラウザ名
		$nBrowserTopic++;
	}

	$sth->finish() or die(DBI::errstr);
	$sth = undef;	# 再利用のため初期化

	# ブラウザ名による１ヶ月ごとアクセステーブル集計
	for($j=0; $j<$nBrowserTopic; $j++)
	{
		for($i=0; $i<$n_stat_month; $i++)
		{
			$nStartEpockSec = $tm_start - 30*24*60*60*($i+1);
			$nEndEpockSec = $tm_start - 30*24*60*60*$i;
			$str_query = "SELECT COUNT (*) FROM acctbl WHERE tm_epock >= ? AND tm_epock <= ? AND browser LIKE ?";

			$sth = $dbh->prepare($str_query) or die(DBI::errstr);
			$sth->bind_param(1, $nStartEpockSec, SQL_INTEGER) or die(DBI::errstr);
			$sth->bind_param(2, $nEndEpockSec, SQL_INTEGER) or die(DBI::errstr);
			$sth->bind_param(3, '%'.$arr_BrowserStatData[$j][0].'%', SQL_VARCHAR) or die(DBI::errstr);
			$sth->execute() or die(DBI::errstr);
			@row = $sth->fetchrow_array();
			$arr_BrowserStatData[$j][$i+2] = $row[0];		# 統計データ

			$sth->finish() or die(DBI::errstr);
			$sth = undef;	# 再利用のため初期化
		}

	}


	##############################
	# OS 集計

	print("info : OS stat processing ...\n");

	# OS集計テーブルを読み込む
	$sth = $dbh->prepare("select * from statostbl") or die(DBI::errstr);
	$sth->execute() or die(DBI::errstr);

	$nOsTopic = 0;
	while(@row = $sth->fetchrow_array())
	{
		$arr_OsStatData[$nOsTopic][0] = $row[1];	# 検索パターン
		$arr_OsStatData[$nOsTopic][1] = $row[2];	# OS名
		$nOsTopic++;
	}

	$sth->finish() or die(DBI::errstr);
	$sth = undef;	# 再利用のため初期化

	# OS名による１ヶ月ごとアクセステーブル集計
	for($j=0; $j<$nOsTopic; $j++)
	{
		for($i=0; $i<$n_stat_month; $i++)
		{
			$nStartEpockSec = $tm_start - 30*24*60*60*($i+1);
			$nEndEpockSec = $tm_start - 30*24*60*60*$i;
			$str_query = "SELECT COUNT (*) FROM acctbl WHERE tm_epock >= ? AND tm_epock <= ? AND browser LIKE ?";

			$sth = $dbh->prepare($str_query) or die(DBI::errstr);
			$sth->bind_param(1, $nStartEpockSec, SQL_INTEGER) or die(DBI::errstr);
			$sth->bind_param(2, $nEndEpockSec, SQL_INTEGER) or die(DBI::errstr);
			$sth->bind_param(3, '%'.$arr_OsStatData[$j][0].'%', SQL_VARCHAR) or die(DBI::errstr);
			$sth->execute() or die(DBI::errstr);
			@row = $sth->fetchrow_array();
			$arr_OsStatData[$j][$i+2] = $row[0];		# 統計データ

			$sth->finish() or die(DBI::errstr);
			$sth = undef;	# 再利用のため初期化
		}

	}


	print("info : popular pages stat processing ...\n");

	##############################
	# アクセス数の多いページ順 集計
	$nStartEpockSec = $tm_start - 30*24*60*60*1;
	$nEndEpockSec = $tm_start;
	$str_query = "SELECT referer, count(1) FROM acctbl WHERE tm_epock >= ? AND tm_epock <= ? GROUP BY referer ORDER BY COUNT(1) DESC LIMIT 20";
	
	$sth = $dbh->prepare($str_query) or die(DBI::errstr);
	$sth->bind_param(1, $nStartEpockSec, SQL_INTEGER) or die(DBI::errstr);
	$sth->bind_param(2, $nEndEpockSec, SQL_INTEGER) or die(DBI::errstr);
	$sth->execute() or die(DBI::errstr);
	$i = 0;
	while(@row = $sth->fetchrow_array())
	{
		$arr_PopularPage[$i][0] = sub_conv_to_flagged_utf8($row[0], 'shiftjis');	# DBはShift JISで記録されているため
		$arr_PopularPage[$i][1] = $row[1];
		$nPopularPages++;
		$i++;
	}
	$sth->finish() or die(DBI::errstr);
	$sth = undef;	# 再利用のため初期化


	# SQL切断
	$dbh->disconnect() or die(DBI::errstr);
	print("info : database close nomally\n");



};
if($@){
	# evalによるDBエラートラップ：エラー時の処理
	if(defined($dbh)){ $dbh->disconnect(); }
	my $str = $@;
	chomp($str);
	print("===SQLERROR (counter) (".$str.")===");

	exit;
}


#**************
# ApacheLog DBからの、総アクセス数の読み込み

$dbh = undef;
eval{

	if($strApachelogSqlDsn ne ''){

		# SQLサーバに接続
		$dbh = DBI->connect($strApachelogSqlDsn, "", "", {PrintError => 1, AutoCommit => 0}) or die(DBI::errstr);

		print("open DB : ".$strApachelogSqlDsn."\n");

		##############################
		# 直近 過去30日間の1日毎のアクセス数
		my $str_query = "SELECT ext_html + ext_php FROM stat WHERE date >= ? AND date <= ?";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		for($i=0; $i<30; $i++)
		{
			$nStartEpockSec = $tm_start - 24*60*60*($i+1);
			$nEndEpockSec = $tm_start - 24*60*60*$i;
			$sth->bind_param(1, $nStartEpockSec, SQL_INTEGER) or die(DBI::errstr);
			$sth->bind_param(2, $nEndEpockSec, SQL_INTEGER) or die(DBI::errstr);
			$sth->execute() or die(DBI::errstr);
			@row = $sth->fetchrow_array();
			$arr_nDayTotalDataAll[$i] = defined($row[0]) ? $row[0] : 0;
		}
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用のため初期化


		# SQL切断
		$dbh->disconnect() or die(DBI::errstr);
		print("info : database close nomally\n");

	}

};
if($@){
	# evalによるDBエラートラップ：エラー時の処理
	if(defined($dbh)){ $dbh->disconnect(); }
	my $str = $@;
	chomp($str);
	print("===SQLERROR (counter) (".$str.")===");

	exit;
}



# クエリにかかった時間
$tm_lapse_query = Time::HiRes::time() - $tm_lapse_query_start;

#**************
# 統計データの書き込み

if(open(OUT, "> $str_filepath_out")){
	binmode(OUT, ":utf8");

# HTMLヘッダ
	print(OUT  "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n".
		"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"ja\" lang=\"ja\">\n".
		"<head>\n".
		"\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n".
		"\t<meta http-equiv=\"Content-Language\" content=\"ja\" />\n".
		"\t<title>access log query</title>\n");

# Google Visualization API スクリプト

	print(OUT  "\t<script type=\"text/javascript\" src=\"http://www.google.com/jsapi\"></script>\n");

	$strTmp = "\t<script type=\"text/javascript\">\n\n".
		"\t// Load the Visualization API\n".
		"\tgoogle.load('visualization', '1', {'packages':['corechart']});\n".
		"\t// Set a callback to run when the Google Visualization API is loaded.\n".
		"\tgoogle.setOnLoadCallback(drawChart);\n\n".
		"\t// Callback that creates and populates a data table, \n".
		"\t// instantiates the pie chart, passes in the data and\n".
		"\t// draws it.\n".
		"\tfunction drawChart() {\n\n".

		"\t\t// Create data_browser table.\n";

	#期間内全アクセス数グラフ
	$strTmp .= "\t\tvar data_allaccess = new google.visualization.arrayToDataTable([\n".
		"\t\t\t['期間', 'アクセス数'],\n";
	for($j=0; $j<$n_stat_month; $j++)
	{
		$strTmp .= "\t\t\t['".($n_stat_month-$j-1)."ヶ月前', ".sprintf("%.1f", $arr_nSubTotalData[$n_stat_month-$j-1]/30.0)."],\n";
	}
	chop($strTmp);   # 最後の改行を除去
	chop($strTmp);   # 最後のコンマを除去
	$strTmp .= "\n\t\t]);\n";

	#過去30日間アクセス数グラフ
	if($strApachelogSqlDsn ne ''){
		$strTmp .= "\t\tvar data_dayaccess = new google.visualization.arrayToDataTable([\n".
			"\t\t\t['日前', '重複排除アクセス数', '総アクセス数'],\n";
		for($j=0; $j<30; $j++)
		{
			$strTmp .= "\t\t\t['-".(30-$j-1)."日', ".$arr_nDayTotalData[30-$j-1].",".$arr_nDayTotalDataAll[30-$j-1]."],\n";
		}
	}
	else{
		$strTmp .= "\t\tvar data_dayaccess = new google.visualization.arrayToDataTable([\n".
			"\t\t\t['日前', '重複排除アクセス数'],\n";
		for($j=0; $j<30; $j++)
		{
			$strTmp .= "\t\t\t['-".(30-$j-1)."日', ".$arr_nDayTotalData[30-$j-1]."],\n";
		}
	}
	chop($strTmp);   # 最後の改行を除去
	chop($strTmp);   # 最後のコンマを除去
	$strTmp .= "\n\t\t]);\n";

		#ブラウザ統計グラフ
	$strTmp .= "\t\tvar data_browser = new google.visualization.arrayToDataTable([\n".
		"\t\t\t['期間', ";
	for($j=0; $j<$nBrowserTopic; $j++)
	{
		$strTmp .= "'".$arr_BrowserStatData[$j][1]."',";
	}
	chop($strTmp);   # 最後のコンマを除去
	$strTmp .= "],\n";

	for($j=0; $j<$n_stat_month; $j++)
	{
		$strTmp .= "\t\t\t['".($n_stat_month-$j-1)."ヶ月前',";
		for($i=0; $i<$nBrowserTopic; $i++)
		{
			$strTmp .= sprintf("%.2f", $arr_BrowserStatData[$i][($n_stat_month-$j-1)+2]/($arr_nSubTotalData[$n_stat_month-$j-1]!=0?$arr_nSubTotalData[$n_stat_month-$j-1]:1000000)*100).",";
		}
		chop($strTmp);   # 最後のコンマを除去
		$strTmp .= "],\n";
	}
	chop($strTmp);   # 最後の改行を除去
	chop($strTmp);   # 最後のコンマを除去
	$strTmp .= "\n\t\t]);\n";

	# OS統計グラフ
	$strTmp .= "\t\tvar data_os = new google.visualization.arrayToDataTable([\n".
		"\t\t\t['期間', ";

	for($j=0; $j<$nOsTopic; $j++)
	{
		$strTmp .= "'".$arr_OsStatData[$j][1]."',";
	}
	chop($strTmp);   # 最後のコンマを除去
	$strTmp .= "],\n";

	for($j=0; $j<$n_stat_month; $j++)
	{
		$strTmp .= "\t\t\t['".($n_stat_month-$j-1)."ヶ月前',";
		for($i=0; $i<$nOsTopic; $i++)
		{
			$strTmp .= sprintf("%.2f", $arr_OsStatData[$i][($n_stat_month-$j-1)+2]/($arr_nSubTotalData[$n_stat_month-$j-1]!=0?$arr_nSubTotalData[$n_stat_month-$j-1]:100000)*100).",";
		}
		chop($strTmp);   # 最後のコンマを除去
		$strTmp .= "],\n";
	}
	chop($strTmp);   # 最後の改行を除去
	chop($strTmp);   # 最後のコンマを除去
	$strTmp .= "\n\t\t]);\n";


	$strTmp .= "\t\t// Instantiate and draw our chart, passing in some options.\n".

		"\t\tvar chart_allaccess = new google.visualization.AreaChart(document.getElementById('google_graph_allaccess'));\n".
		"\t\tchart_allaccess.draw(data_allaccess, {width: 700, height: 200, is3D: true, backgroundColor: '".$strHtmlBackgroundColor."', legendBackgroundColor: '".$strHtmlBackgroundColor."', axisColor: '#e5ad60', focusBorderColor: '#ff0000', colors: ['red'], legendFontSize: 10, titleFontSize: 18, axisFontSize: 12, title: '1ヶ月毎の日平均アクセス数', titleY: '(1日平均)'});\n\n".

		"\t\tvar chart_dayaccess = new google.visualization.LineChart(document.getElementById('google_graph_dayaccess'));\n".
		"\t\tchart_dayaccess.draw(data_dayaccess, {width: 700, height: 200, is3D: true, backgroundColor: '".$strHtmlBackgroundColor."', legendBackgroundColor: '".$strHtmlBackgroundColor."', axisColor: '#e5ad60', focusBorderColor: '#ff0000', colors: ['red','orange'], legendFontSize: 10, titleFontSize: 18, axisFontSize: 12, title: '直近30日間の日毎アクセス数', titleY: '(件)'});\n\n".

		"\t\tvar chart_browser = new google.visualization.LineChart(document.getElementById('google_graph_browser'));\n".
		"\t\tchart_browser.draw(data_browser, {width: 700, height: 400, is3D: true, backgroundColor: '".$strHtmlBackgroundColor."', legendBackgroundColor: '".$strHtmlBackgroundColor."', axisColor: '#e5ad60', focusBorderColor: '#ff0000', legendFontSize: 10, titleFontSize: 18, axisFontSize: 12, title: 'ブラウザ種別統計', titleY: '(%)'});\n\n".
		
		"\t\tvar chart_os = new google.visualization.LineChart(document.getElementById('google_graph_os'));\n".
		"\t\tchart_os.draw(data_os, {width: 700, height: 400, is3D: true, backgroundColor: '".$strHtmlBackgroundColor."', legendBackgroundColor: '".$strHtmlBackgroundColor."', axisColor: '#e5ad60', focusBorderColor: '#ff0000', legendFontSize: 10, titleFontSize: 18, axisFontSize: 12, title: 'OS種別統計', titleY: '(%)'});\n\n".
		
		"\t}\n".
		"\t</script>\n";

	print(OUT  $strTmp);

	print(OUT  "\n\t<style type=\"text/css\">\n".
				"\t\tbody {\n".
				"\t\t\tfont-family: sans-serif;\n".
				"\t\t\tfont-size: 16px;\n".
				"\t\t\tbackground-color: ".$strHtmlBackgroundColor.";\n".
				"\t\t\tposition: relative;\n".
				"\t\t\tmargin: 0 auto;\n".
				"\t\t\tpadding: 0;\n".
				"\t\t\twidth: 800px;\n".
				"\t\t}\n\n".
				"\t\tH1 {\n".
				"\t\t\tfont-size: 16pt;\n".
				"\t\t\tfont-weight: lighter;\n".
  				"\t\t\ttext-indent: 5px;\n".
				"\t\t\tletter-spacing: 8px;\n".
				"\t\t\tline-height: 200%;\n".
				"\t\t\tmargin-bottom: 50px;\n".
				"\t\t\tborder-width: 0px 0px 1px 0px;\n".
				"\t\t\tborder-style: solid;\n".
				"\t\t\tborder-bottom-color: ".$strHtmlH1LineColor.";\n".
				"\t\t}\n\n".
				"\t\tH2 {\n".
				"\t\t\tfont-size: 14pt;\n".
				"\t\t\tfont-weight: lighter;\n".
  				"\t\t\ttext-indent: 5px;\n".
				"\t\t\tletter-spacing: 3px;\n".
				"\t\t\tline-height: 150%;\n".
				"\t\t\tborder-width: 0px 0px 1px 1px;\n".
				"\t\t\tborder-style: solid;\n".
				"\t\t\tborder-bottom-color: ".$strHtmlH2LineColor.";\n".
				"\t\t\tborder-left-color: ".$strHtmlH2LineColor.";\n".
				"\t\t}\n\n".
				"\t\ttable {\n".
#				"\t\t\tborder: 1px rgb(153, 51, 51) solid;\n".
				"\t\t\tborder: 1px ".$strHtmlTableGridColor." solid;\n".
				"\t\t\tborder-collapse: collapse;\n".
				"\t\t\tfont-size: 10pt;\n".
				"\t\t}\n".
				"\t\ttd, th {\n".
				"\t\t\tborder: 1px ".$strHtmlTableGridColor." solid;\n".
				"\t\t\tpadding: 2px;\n".
				"\t\t}\n".
				"\t</style>\n\n");

	print(OUT  "</head>\n<body>\n");

# 本文開始

	print(OUT  "<h1>アクセスログ解析結果</h1>\n");
	printf(OUT  "<p>集計期間 : %04d年% 02d月 %02d日から過去 %2dヶ月の 1ヶ月ごとの集計データ</p>\n<p>&nbsp;</p>",(localtime($tm_start))[5]+1900, (localtime($tm_start))[4]+1, (localtime($tm_start))[3], $n_stat_month);

	print(OUT  "<h2>全アクセス集計</h2>\n");
	
	print(OUT  "<p>過去".$n_stat_month."ヶ月の日平均アクセス数</p>\n");
	print(OUT "<table>\n\t<tr>\n\t<td>&nbsp;</td>");
	for($i=0; $i<$n_stat_month; $i++)
	{
		print(OUT  "<td>".($n_stat_month-$i-1)."ヶ月前</td>");
	}
	print(OUT "\n\t</tr>\n\t<tr>\n\t<td>&nbsp;</td>");
	for($i=0; $i<$n_stat_month; $i++)
	{
		print(OUT  "<td>".$arr_nSubTotalData[$n_stat_month-$i-1]."</td>");
	}
	print(OUT  "\n\t</tr>\n</table>\n");
	print(OUT  "<p>&nbsp;</p>\n");
	print(OUT  "<div id=\"google_graph_allaccess\"></div>\n");
	print(OUT  "<p>&nbsp;</p>\n");

	# 過去30日間の日毎データ
	print(OUT  "<p>過去30日の日毎アクセス数</p>\n");
	print(OUT "<table>\n\t<tr>\n\t<td>日前</td>");
	for($i=0; $i<30; $i++)
	{
		print(OUT  "<td>".(30-$i-1)."</td>");
	}
	print(OUT "\n\t</tr>\n\t<tr>\n\t<td>&nbsp;</td>");
	for($i=0; $i<30; $i++)
	{
		print(OUT  "<td>".$arr_nDayTotalData[30-$i-1]."</td>");
	}
	print(OUT  "\n\t</tr>\n</table>\n");
	print(OUT  "<p>&nbsp;</p>\n");
	print(OUT  "<div id=\"google_graph_dayaccess\"></div>\n");
	if($strApachelogSqlDsn ne ''){
		print(OUT "<p style=\"font-size:8pt;\">総アクセス数：Apacheのアクセスログよりhtmlとphpファイルのアクセス数合計<br />重複排除アクセス数：同一IPアドレスからの連続アクセスを重複カウントしないアクセス数</p>\n");
	}
	print(OUT  "<p>&nbsp;</p>\n");

	print(OUT  "<h2>ブラウザ集計</h2>\n");
	
	print(OUT "<table>\n");
	print(OUT  "\t<tr>\n\t<td>ブラウザ</td>");
	for($i=0; $i<$n_stat_month; $i++)
	{
		print(OUT  "<td>".($n_stat_month-$i-1)."ヶ月前</td>");
	}
	print(OUT  "\n\r</tr>\n");
	for($j=0; $j<$nBrowserTopic; $j++)
	{
		print(OUT  "\t<tr>\n\t<td>".$arr_BrowserStatData[$j][1]."</td>");
		for($i=0; $i<$n_stat_month; $i++)
		{
			print(OUT  "<td>".$arr_BrowserStatData[$j][($n_stat_month-$i-1)+2]."</td>");
		}
		print(OUT  "\n\t</tr>\n");
	}
	print(OUT  "</table>\n");
	print(OUT  "<p>&nbsp;</p>\n");
	print(OUT  "<div id=\"google_graph_browser\"></div>\n");
	print(OUT  "<p>&nbsp;</p>\n");

	print(OUT  "<h2>OS集計</h2>\n");

	print(OUT "<table>\n");
	print(OUT  "\t<tr>\n\t<td>OS名</td>");
	for($i=0; $i<$n_stat_month; $i++)
	{
		print(OUT  "<td>".($n_stat_month-$i-1)."ヶ月前</td>");
	}
	print(OUT  "\n\r</tr>\n");
	for($j=0; $j<$nOsTopic; $j++)
	{
		print(OUT  "\t<tr>\n\t<td>".$arr_OsStatData[$j][1]."</td>");
		for($i=0; $i<$n_stat_month; $i++)
		{
			print(OUT  "<td>".$arr_OsStatData[$j][($n_stat_month-$i-1)+2]."</td>");
		}
		print(OUT  "\n\t</tr>\n");
	}
	print(OUT  "</table>\n");
	print(OUT  "<p>&nbsp;</p>\n");
	print(OUT  "<div id=\"google_graph_os\"></div>\n");
	print(OUT  "<p>&nbsp;</p>\n");

	print(OUT  "<h2>アクセス数の多いページ（直近1ヶ月）</h2>\n");
	print(OUT "<table>\n\t<tr>\n\t<td>HTMLファイル名</td><td>アクセス数</td>\n\t</tr>\n");
		for($i=0; $i<$nPopularPages; $i++)
		{
			print(OUT  "\t<tr>\n\t<td>".$arr_PopularPage[$i][0]."</td><td>".$arr_PopularPage[$i][1]."</td>\n\t</tr>\n");
		}
	print(OUT  "</table>\n");
	
	print(OUT  "<p>&nbsp;</p>\n<h2>備考</h2>\n".
			"<p>&nbsp;</p>\n<p>データベース クエリに要した処理時間 ".$tm_lapse_query."秒</p>\n");

	print(OUT  "<p>&nbsp;</p>\n".
			"<p>このアクセス解析は、UNIXのcronを用いて起動されるPerlスクリプトにより、一定期間ごとに自動的に生成されています。</p>\n".
			"<p>アクセス解析はコンテンツの主要ページを訪問したユーザのブラウザから返される環境変数を用いて集計されています。　一度訪問したユーザのIPアドレスは一定期間保存してチェックされ、1度しか集計されない仕組みになっています。（複数のページにアクセスしても、最初のページしか検出していません）</p>\n".
			"<p>&nbsp;</p>\n<p>Powered by ..</p>\n".
			"<ul>\n".
			"<li><a href=\"http://www.sqlite.org/\">SQLite3</a></li>\n".
			"<li><a href=\"http://www.perl.com/\">Perl 5</a></li>\n".
			"<li><a href=\"http://search.cpan.org/dist/DBI/DBI.pm\">CPAN DBI</a></li>\n".
			"<li><a href=\"https://google-developers.appspot.com/chart/interactive/docs/gallery\">Google Chart Tools</a></li>\n".
			"</ul>\n");

	print(OUT  "</body>\n");

		print(OUT  "</html>\n");

	close(OUT);
}

exit();


# *************
# 文字コード相互変換サブルーチン

# 任意の文字コードの文字列を、UTF-8フラグ付きのUTF-8に変換する
sub sub_conv_to_flagged_utf8{
		my $str = shift;
		my $enc_force = undef;
		if(@_ >= 1){ $enc_force = shift; }			  # デコーダの強制指定
		
		# デコーダが強制的に指定された場合
		if(defined($enc_force)){
				if(ref($enc_force)){
						$str = $enc_force->decode($str);
						return($str);
				}
				elsif($enc_force ne '')
				{
						$str = Encode::decode($enc_force, $str);
				}
		}

		my $enc = Encode::Guess->guess($str);   # 文字列のエンコードの判定

		unless(ref($enc)){
				# エンコード形式が2個以上帰ってきた場合 （shiftjis or utf8）
				my @arr_encodes = split(/ /, $enc);
				if(grep(/^$flag_charcode/, @arr_encodes) >= 1){
						# $flag_charcode と同じエンコードが検出されたら、それを優先する
						$str = Encode::decode($flag_charcode, $str);
				}
				elsif(lc($arr_encodes[0]) eq 'shiftjis' || lc($arr_encodes[0]) eq 'euc-jp' || 
						lc($arr_encodes[0]) eq 'utf8' || lc($arr_encodes[0]) eq 'us-ascii'){
						# 最初の候補でデコードする
						$str = Encode::decode($arr_encodes[0], $str);
				}
		}
		else{
				# UTF-8でUTF-8フラグが立っている時以外は、変換を行う
				unless(ref($enc) eq 'Encode::utf8' && utf8::is_utf8($str) == 1){
						$str = $enc->decode($str);
				}
		}

		return($str);
}


# 任意の文字コードの文字列を、UTF-8フラグ無しのUTF-8に変換する
sub sub_conv_to_unflagged_utf8{
		my $str = shift;

		# いったん、フラグ付きのUTF-8に変換
		$str = sub_conv_to_flagged_utf8($str);

		return(Encode::encode('utf8', $str));
}


# UTF8から現在のOSの文字コードに変換する
sub sub_conv_to_local_charset{
		my $str = shift;

		# UTF8から、指定された（OSの）文字コードに変換する
		$str = Encode::encode($flag_charcode, $str);
		
		return($str);
}


# 引数で与えられたファイルの エンコードオブジェクト Encode::encode を返す
sub sub_get_encode_of_file{
		my $fname = shift;			  # 解析するファイル名

		# ファイルを一気に読み込む
		open(FH, "<".sub_conv_to_local_charset($fname));
		my @arr = <FH>;
		close(FH);
		my $str = join('', @arr);			   # 配列を結合して、一つの文字列に

		my $enc = Encode::Guess->guess($str);   # 文字列のエンコードの判定

		# エンコード形式の表示（デバッグ用）
		print("Automatick encode ");
		if(ref($enc) eq 'Encode::utf8'){ print("detect : utf8\n"); }
		elsif(ref($enc) eq 'Encode::Unicode'){
				print("detect : ".$$enc{'Name'}."\n");
		}
		elsif(ref($enc) eq 'Encode::XS'){
				print("detect : ".$enc->mime_name()."\n");
		}
		elsif(ref($enc) eq 'Encode::JP::JIS7'){
				print("detect : ".$$enc{'Name'}."\n");
		}
		else{
				# 二つ以上のエンコードが推定される場合は、$encに文字列が返る
				print("unknown (".$enc.")\n");
		}

		# エンコード形式が2個以上帰ってきた場合 （例：shiftjis or utf8）でテクと失敗と扱う
		unless(ref($enc)){
				$enc = '';
		}

		# ファイルがHTMLの場合 Content-Type から判定する
		if(lc($fname) =~ m/html$/ || lc($fname) =~ m/htm$/){
				my $parser = HTML::HeadParser->new();
				unless($parser->parse($str)){
						my $content_enc = $parser->header('content-type');
						if(defined($content_enc) && $content_enc ne '' && lc($content_enc) =~ m/text\/html/ ){
								if(lc($content_enc) =~ m/utf-8/){ $enc = 'utf8'; }
								elsif(lc($content_enc) =~ m/shift_jis/){ $enc = 'shiftjis'; }
								elsif(lc($content_enc) =~ m/euc-jp/){ $enc = 'euc-jp'; }
								
								print("HTML Content-Type detect : ". $content_enc ." (is overrided)\n");
								$enc = $content_enc;
						}
				}
		}

		return($enc);
}
