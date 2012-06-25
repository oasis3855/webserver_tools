#!/usr/bin/perl

# save this file in << utf8  >> encode !

use strict;

# 日付オブジェクトを用いる
use Time::Local;
# データベースオブジェクトを用いる
use DBI;

use URI::Escape;
use Encode;
use Encode::Guess;
use File::Basename;
use FindBin qw/$Bin/;	# サーバ上でのフルパス名を得るため

# ミリ秒を利用する
use Time::HiRes;

# 出力CSVファイルを決定する変数
my $nTargetYear = 0;
my $strCsvFilename = $FindBin::Bin.'/../backup/';

# データベース ハンドル
my $dbh = undef;
my $sth = undef;

# 変数
my $nTotalData = 0;		# データベース全体での全データ数

# 一時変数
my $strQuery = undef;	# クエリを行うための一時的なクエリ命令文

my $strTmp = "";
my $nStartEpockSec = 0;
my $nEndEpockSec = 0;
my @arrWdayStr = ("Sun","Mon","Tue","Wed","Thu","Fri","Sat");

my $i = 0;
my $j = 0;

my @row = ();		# クエリ結果を受ける配列

# SQLサーバのDSN定義（DB名は対話的に入力処理を行う）
my $strDbPath = $FindBin::Bin.'/../data/';
my $strSqlDsn = 'DBI:SQLite:dbname='.$FindBin::Bin.'/../data/';
my $strDefaultDB = 'accdb.sqlite';

# 初期メッセージの表示
print("\n=== Export access data to CSV file ===\n");

# 引数（対象年）の処理。またはコンソールから対象年、DB名を読み込む
if($#ARGV != 0) {
	# プログラムへの引数がない場合

	$strDefaultDB = sub_select_dbfile($strDbPath);	# 対象 DB ファイルの選択
	$strSqlDsn .= $strDefaultDB;

	# 対象年の対話的入力
	print("input target year (from 2000 to 2032) : ");
	$_ = <STDIN>;
	chomp();		# 行末の改行を取る
	$nTargetYear = int($_);		# 整数を取り出す

}
else
{
	# 引数があれば、それを対象年として読み込む
	$nTargetYear = sprintf("%d", $ARGV[0]);
	
	# 標準のDB名を用いる
	$strSqlDsn .= $strDefaultDB;
}

# ターゲット年、出力ファイル名を確定
$strCsvFilename = $strCsvFilename . "backup". $nTargetYear . ".csv";
print("target DB :". $strSqlDsn . "\n");
print("target year :". $nTargetYear . "\n");
print("output csv :". $strCsvFilename . "\n");

if($nTargetYear < 2000 || $nTargetYear > 2032)
{
	print("error : target year must be within 2000 and 2032\n");
	exit();
}

if(-e $strCsvFilename)
{
	print("warning : CSV file exists\noverwrite ? (y/n): ");
	$_ = <STDIN>;
	chomp();	# 行末の改行を取る
	if(uc($_) ne "Y")
	{
		print("info : cancel\n");
		exit();
	}
	print("info : overwrite\n");
}
else
{
	print("info : create NEW csv file\n");
}

print("\nimportant ! : sure to EXPORT ? (y/n): ");
$_ = <STDIN>;
chomp();	# 行末の改行を取る
if(uc($_) ne "Y")
{
	print("info : cancel\n");
	exit();
}


# UNIX時間で開始・終了秒を特定
$nStartEpockSec = timelocal(0,0,0,1,0,$nTargetYear-1900);
$nEndEpockSec = timelocal(59,59,23,31,11,$nTargetYear-1900);

print("target time span : from ".$nStartEpockSec." to ".$nEndEpockSec."\n");

eval{

	# SQLサーバに接続
	$dbh = DBI->connect($strSqlDsn, "", "", {PrintError => 1, AutoCommit => 0});

	if(!$dbh){
		print("error : database open error\n");
		exit;
	}

	# データベース全体のデータ数
	$strQuery = "SELECT COUNT (*) FROM acctbl WHERE tm_epock >= '".$nStartEpockSec."' AND tm_epock <= '".$nEndEpockSec."'";
	$sth = $dbh->prepare($strQuery);
	if($sth){ $sth->execute();}
	@row = $sth->fetchrow_array();
	$nTotalData = $row[0];

	if($sth){ $sth->finish();}

	print("info : ".$nTotalData." lines is exporting ...\n");

	# データを読み出して、CSVに出力する
	$strQuery = "SELECT * FROM acctbl WHERE tm_epock >= '".$nStartEpockSec."' AND tm_epock <= '".$nEndEpockSec."' ORDER BY tm_epock";
	print("info : SQL = ".$strQuery."\n");
	$sth = $dbh->prepare($strQuery);
	if($sth){ $sth->execute();}

	if(open(OUT, "> $strCsvFilename")){

		while(@row = $sth->fetchrow_array())
		{
			my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime($row[1]);

			printf(OUT  "%d,%d,%d,%d,%d,%d,%d,%s,%s,,,%s,%s,%s,\n", 
				$row[0], $year+1900, $mon+1, $mday, $hour, $min, $sec,
				$row[5], $row[4], $row[6], $row[7], $row[8]);
			
		}

	close(OUT);
}

	if($sth){ $sth->finish();}


	# SQL切断
	$dbh->disconnect();
	print("info : DB close nomally\n");

};
if($@){
# evalによるエラートラップ：エラー時の処理
#	$dbh->rollback;
	$dbh->disconnect();
	print("\nerror : SQLite message = ".$@."\n");
	exit();
}

# 結果出力

$i = -s $strCsvFilename;
print("\ninfo : export complete !  (".$i." bytes written)\n");


exit();

# SQLite データベースファイル一覧より、対象ファイルを選択する
sub sub_select_dbfile {
	my $strSearchPath = shift;
	$strSearchPath .= '*.sqlite';

	my @arrScan = glob($strSearchPath);

	if($#arrScan < 0){ die("Cannot find SQLite DB data file, at $strSearchPath \n"); }

	print("database file list\n");
	my $i=0;
	foreach(@arrScan){
		print(" ".++$i." : ".basename($_)."\n");
	}
	print("select target database from 1 to ".$i.". (1-".$i.") : ");
	$_ = <STDIN>;
	chomp();
	if(int($_)<1 || int($_)>$i){ die("input is out of range\n"); }
	
	my $strSelectedFile = basename($arrScan[int($_)-1]);

	print("user select : ".$strSelectedFile."\n");
	
	return($strSelectedFile);

}

