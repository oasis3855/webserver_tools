#!/usr/bin/perl

# save this file in << utf8  >> encode !

use strict;

# 日付オブジェクトを用いる
use Time::Local;
# データベースオブジェクトを用いる
use DBI;

use URI::Escape;
use File::Basename;
use Encode;
use Encode::Guess;
use FindBin qw/$Bin/;	# サーバ上でのフルパス名を得るため

# ミリ秒を利用する
use Time::HiRes;

# 入力CSVファイルを決定する変数
my $nTargetYear = 0;
my $strCsvFilename = $FindBin::Bin.'/../backup/';

# データベース ハンドル
my $dbh = undef;
my $sth = undef;

# 変数
my $nTotalData = 0;		# データベース全体での全データ数


# 作業用変数
my $strQuery = undef;	# クエリを行うための一時的なクエリ命令文
my @row = ();		# クエリ結果を受ける配列

my $strTmp = "";
my @arrayTmp;		# CSVファイルの行データを切り分けた配列
my $tmTmp = timelocal(0,0,0,1,0,0);
my $tm2000 = timelocal(0,0,0,1,0,2000-1900);	# 2000/1/1 00:00
my $tm2032 = timelocal(0,0,0,1,0,2032-1900);	# 2032/1/1 00:00
my $nWday = 0;
my $nDaySeconds = 0;

my $i = 0;
my $j = 0;


my $tmQueryTime = undef;		# クエリにかかった時間
my $tmQueryStartTime = undef;	# クエリ開始時刻
my $tmQueryEndTime = undef;		# クエリ終了時刻

# SQLサーバのDSN定義
my $strDbPath = $FindBin::Bin.'/../data/';
my $strSqlDsn = 'DBI:SQLite:dbname='.$FindBin::Bin.'/../data/';
my $strDefaultDB = 'accdb.backup.sqlite';

print("\n=== Import access data from CSV file ===\n");


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
print("import from csv :". $strCsvFilename . "\n");

if($nTargetYear < 2000 || $nTargetYear > 2032)
{
	print("error : target year must be within 2000 and 2032\n");
	exit();
}

# CSVファイルの存在を確認
if(!(-r $strCsvFilename))
{
	print("error: CSV file is not exist or not readable\n");
	exit();
}

print("\nimportant ! : sure to IMPORT ? (y/n): ");
$_ = <STDIN>;
chomp();	# 行末の改行を取る
if(uc($_) ne "Y")
{
	print("info : cancel\n");
	exit();
}


# クエリにかかる時間を計測するため、開始時刻を保存
$tmQueryStartTime = Time::HiRes::time();

# SQLサーバに接続
$dbh = DBI->connect($strSqlDsn, "", "", {PrintError => 1, AutoCommit => 0}) or die("error : database open error\n");

eval{

	if(open(IN, "< $strCsvFilename"))
	{

		$i = 0;		# データ数カウンタ

		while(<IN>) {	#********* WHILE LOOP START HERE *********
			$strTmp = $_;
			if(length($strTmp) < 10){ next;}


	#	CSVは次のように配列に入力される
	#	$count,$year,$mon,$mday,$hour,$min,$sec,$proxy_name,$remip,$userip,$proxy_str,$browser,$referer,$lang,$last_acc_log

			@arrayTmp = split(/,/, $_);

			#配列要素が8個未満（IPアドレス項目が無い）のときは、その行をスキップ
			if($#arrayTmp < 8) { next;}

			# 日時をUNIX時刻に変換
			$tmTmp = timelocal($arrayTmp[6], $arrayTmp[5], $arrayTmp[4], $arrayTmp[3], $arrayTmp[2]-1, $arrayTmp[1]-1900);

			#日時がおかしい場合（2000年以前、2032年以降）、その行をスキップ
			if($tmTmp < $tm2000 || $tmTmp > $tm2032) { next;}

			# 曜日を求める
			$nWday = (localtime($tmTmp))[6];
		
			# その日の0時からの秒数
			$nDaySeconds = $arrayTmp[4]*60*60 + $arrayTmp[5]*60 + $arrayTmp[6];

			# 既に登録されているデータでないか1000行おきにチェック
			if($i % 1000 == 0)
			{
				printf("info : processing ".$i." data ...\n");	# 経過メッセージ表示

				$strQuery = "select count (*) from acctbl where tm_epock == '".$tmTmp."' and ip == '".$arrayTmp[8]."'";
				$sth = $dbh->prepare($strQuery);
				if($sth){ $sth->execute();}
				my @row = $sth->fetchrow_array();
				if($row[0] != 0)
				{
					printf("\nsame data is detected on date=%04d/%02d/%02d %02d:%02d:%02d ip=%s . force continue ? (y/n): ",
						$arrayTmp[1], $arrayTmp[2], $arrayTmp[3], $arrayTmp[4], $arrayTmp[5], $arrayTmp[6], $arrayTmp[8]);
					$_ = <STDIN>;
					chomp();	# 行末の改行を取る
					if(uc($_) ne "Y")
					{
						if($sth){ $sth->finish();}
						# DBをロールバックして処理終了する
						$dbh->rollback;
						$dbh->disconnect();
						print("error : same data exist\n");
						exit();
					}
				}
				if($sth){ $sth->finish();}
			}

			# SQL文を構築する
			$strQuery = "insert into acctbl values(null,".$tmTmp.",".$nWday.",".$nDaySeconds.",'".$arrayTmp[8]."','".&SanitizeString($arrayTmp[7],255)."','".&SanitizeString($arrayTmp[11],255)."','".&SanitizeString($arrayTmp[12],255)."','".$arrayTmp[13]."')";
			
			##### DEBUG
	#		print("info: ".$strSQL."\n");
			##### DEBUG

			$sth = $dbh->prepare($strQuery);
			if($sth){ $sth->execute();}
			if($sth){ $sth->finish();}
			$i++;			# データ数カウンタ

		}
		
		# 書き込みのコミット
		print("info : DB commit\n");
		$dbh->commit;

		# CSVファイルを閉じる
		close(IN);
	}

	# SQL切断
	$dbh->disconnect();
	print("info : DB close nomally\n");

};
if($@){
# evalによるエラートラップ：エラー時の処理
	$dbh->rollback;
	$dbh->disconnect();
	print("\nError : ".$@."\n");
	exit();
}

# クエリにかかる時間を計測するため、終了時刻を保存
$tmQueryEndTime = Time::HiRes::time();
# クエリにかかった時間
$tmQueryTime = $tmQueryEndTime - $tmQueryStartTime;


print("info : ".$i." lines added, process time is ".$tmQueryTime." sec\n");

# 総データ数を得る（表示用）
eval{
	# SQLサーバに接続
	$dbh = DBI->connect($strSqlDsn, "", "", {PrintError => 1, AutoCommit => 0});
	if(!$dbh){
		print("error : database open error\n");
		exit;
	}

	$sth = $dbh->prepare("SELECT COUNT (*) FROM acctbl");
	if($sth){ $sth->execute();}
	my @row = $sth->fetchrow_array();
	if(@row)
	{
		print("info : total data is ".$row[0]." lines\n");
	}
	if($sth){ $sth->finish();}

	$dbh->disconnect();
	print("info : DB close nomally\n");

};
if($@){
# evalによるエラートラップ：エラー時の処理
	$dbh->disconnect();
	print("Error : ".$@."\n");
	exit();
}



exit();


# SQL格納に有害になる文字を除去またはエンコードする
# SanitizeString(char *string, int maxlength);
sub SanitizeString
{
	# ローカル変数に引数を代入
	my($strTmp) = $_[0];

	my $enc = guess_encoding($strTmp, qw/euc-jp shiftjis 7bit-jis/);
	if(!ref($enc))
	{	# UTF8は常に判定エラーになるため
		$strTmp = encode('shiftjis', Encode::decode('utf8', $strTmp));
	}
	else
	{
		$strTmp = encode('shiftjis', decode($enc->name, $strTmp));
	}

	$strTmp =~ s/,//eg;				# コンマ除去
	$strTmp =~ s/\x27//eg;			# シングルクオート（’）除去
	$strTmp =~ s/\x22//eg;			# ダブルクオート（”）除去
	$strTmp =~ tr/\x0-\x1f//d;		# 0x00〜0x1fのバイナリ除去
	
	# 文字列末端にCR/LFがある場合は除去する
#	chomp($strTmp);

	# URLエンコード
#	$strTmp = uri_escape($strTmp);
	# シングルクオート（'）等が標準ではエンコードされないことの対処
#	$strTmp = uri_escape($strTmp, "'");

	# URLエンコードを最小限だけ行う
	$strTmp = uri_escape($strTmp, "%|");

	# 文字列を最大長さで切り捨てる
	$strTmp = substr($strTmp, 0, $_[1]);
	
	return $strTmp;
}


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

